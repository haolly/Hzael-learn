#include "hazelPCH.h"
#include "Renderer2D.h"

#include "Buffer.h"
#include "Material.h"
#include "OrthographicCamera.h"
#include "Pipeline.h"
#include "RenderCommandBuffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "SubTexture2D.h"
#include "UniformBuffer.h"
#include "UniformBufferSet.h"
#include "glm/gtx/transform.hpp"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		// Editor only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		float Thickness;
		glm::vec2 LocalPosition;
		glm::vec4 Color;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 2000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; //TODO, renderCap capability

		Ref<Pipeline> QuadPipeline;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<IndexBuffer> QuadIndexBuffer;
		Ref<Material> QuadMaterial;

		Ref<Texture2D> WhiteTexture;

		Ref<Shader> TextureShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 == white texture

		glm::vec4 QuadVertexPosition[4];

		// Lines
		Ref<Pipeline> LinePipeline;
		Ref<Pipeline> LineOnTopPipeline;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<IndexBuffer> LineIndexBuffer;
		Ref<Material> LineMaterial;

		uint32_t LineIndexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;
		float LineWidth = 1.0f;


		Renderer2D::Statistics Stats;

		glm::mat4 CameraViewProj;
		bool DepthTest = true;

		Ref<UniformBuffer> CameraUniformBuffer;
		Ref<RenderCommandBuffer> CommandBuffer;
		Ref<UniformBufferSet> UniformBufferSet;

		struct UBCamera
		{
			glm::mat4 ViewProjection;
		};
	};

	//static Renderer2DData* s_Data = nullptr;
	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNC();
		//s_Data = new Renderer2DData();

		FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = {
			FramebufferTextureSpecification(ImageFormat::RGBA32F), FramebufferTextureSpecification(ImageFormat::Depth)
		};
		framebufferSpec.Samples = 1;
		framebufferSpec.ClearColor = {0.1f, 0.5f, 0.5f, 0.1f};

		Ref<Framebuffer> framebuffer = Framebuffer::Create(framebufferSpec);

		RenderPassSpecification renderPassSpec;
		renderPassSpec.TargetFramebuffer = framebuffer;
		renderPassSpec.DebugName = "Renderer2D";
		Ref<RenderPass> renderPass = RenderPass::Create(renderPassSpec);

		{
			PipelineSpecification pipelineSpecification;
			pipelineSpecification.DebugName = "Renderer2D-Quad";
			pipelineSpecification.Shader = Renderer::GetShaderLibrary()->Get("Renderer2D");
			pipelineSpecification.RenderPass = renderPass;
			pipelineSpecification.BackfaceCulling = false;
			pipelineSpecification.Layout = {
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float4, "a_Color"},
				{ShaderDataType::Float2, "a_TexCoord"},
				{ShaderDataType::Float, "a_TexIndex"},
				{ShaderDataType::Float, "a_TilingFactor"},
			};
			s_Data.QuadPipeline = Pipeline::Create(pipelineSpecification);
			s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
			s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

			uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
			uint32_t offset = 0;
			for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
			{
				quadIndices[i + 0] = offset + 0;
				quadIndices[i + 1] = offset + 1;
				quadIndices[i + 2] = offset + 2;

				quadIndices[i + 3] = offset + 2;
				quadIndices[i + 4] = offset + 3;
				quadIndices[i + 5] = offset + 0;
				offset += 4;
			}
			s_Data.QuadIndexBuffer = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
			delete[] quadIndices;
		}

		s_Data.WhiteTexture = Renderer::GetWhiteTexture();
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		// Quad origin is zero(screen center), size 1 by 1
		s_Data.QuadVertexPosition[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
		s_Data.QuadVertexPosition[1] = {0.5f, -0.5f, 0.0f, 1.0f};
		s_Data.QuadVertexPosition[2] = {0.5f, 0.5f, 0.0f, 1.0f};
		s_Data.QuadVertexPosition[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

		s_Data.CommandBuffer = RenderCommandBuffer::Create(0, "Renderer2D");
		// todo
		s_Data.UniformBufferSet = UniformBufferSet::Create(3);
		s_Data.UniformBufferSet.Create(sizeof(Renderer2DData::UBCamera), 0);
		s_Data.QuadMaterial = Material::Create(s_Data.QuadPipeline->GetSpecification().Shader, "QuadMaterial");
	}

	void Renderer2D::Shutdown()
	{
	}

	// Start a new draw call
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		int32_t samplers[Renderer2DData::MaxTextureSlots];
		for (int i = 0; i < Renderer2DData::MaxTextureSlots; ++i)
		{
			samplers[i] = i;
		}


		s_Data.TextureShader->Bind();
		s_Data.TextureShader.As<OpenGLShader>()->UploadUniformIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);
		s_Data.TextureShader.As<OpenGLShader>()->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		StartBatch();
	}

	void Renderer2D::BeginScene(const glm::mat4& viewProj, bool depthTest)
	{
		s_Data.CameraViewProj = viewProj;
		s_Data.DepthTest = depthTest;

		Renderer::Submit([viewProj]() mutable
		{
			uint32_t bufferIndex = Renderer::GetCurrentFrameIndex();
			s_Data.UniformBufferSet->Get(0, 0, bufferIndex)->RT_SetData(&viewProj, sizeof(Renderer2DData::UBCamera));
		});

		StartBatch();
	}

	Ref<RenderPass> Renderer2D::GetTargetRenderPass()
	{
	}

	void Renderer2D::SetTargetRenderPass(Ref<RenderPass> renderPass)
	{
	}

	void Renderer2D::FlushAndResetLines()
	{
	}

	void Renderer2D::EndScene()
	{
		s_Data.CommandBuffer->Begin();
		Renderer::BeginRenderPass(s_Data.CommandBuffer, s_Data.QuadPipeline->GetSpecification().RenderPass);

		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		if (dataSize)
		{
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
			Renderer::RenderGeometry(s_Data.CommandBuffer, s_Data.QuadPipeline, s_Data.UniformBufferSet, s_Data.QuadMaterial,
			                         s_Data.QuadVertexBuffer, s_Data.QuadIndexBuffer,
			                         glm::mat4(1.0f), s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}
		Renderer::EndRenderPass(s_Data.CommandBuffer);
		s_Data.CommandBuffer->End();
		s_Data.CommandBuffer->Submit();
	}

	void Renderer2D::Flush()
	{
	}

	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[quadVertexCount] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
		};
		for (int i = 0; i < quadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor,
	                          const glm::vec4& tintColor, int entityID)
	{
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		float textureIndex = 0.0f;
		for (int i = 1; i < s_Data.TextureSlotIndex; ++i)
		{
			if (*s_Data.TextureSlots[i].Raw() == *texture.Raw())
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		const glm::vec4& color = tintColor;

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[quadVertexCount] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
		};
		for (int i = 0; i < quadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor,
	                          const glm::vec4& tintColor)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor,
	                          const glm::vec4& tintColor)
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture,
	                          float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, subTexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture,
	                          float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNC();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		auto texture = subTexture->GetTexture();

		float textureIndex = 0.0f;
		for (int i = 1; i < s_Data.TextureSlotIndex; ++i)
		{
			if (*s_Data.TextureSlots[i].Raw() == *texture.Raw())
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

		const glm::vec4& color = tintColor;


		constexpr size_t quadVertexCount = 4;
		const glm::vec2* textureCoords = subTexture->GetTextureCoords();
		for (int i = 0; i < quadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, float rotation, const glm::vec2& size,
	                                 const glm::vec4& color)
	{
		DrawRotatedQuad({position.x, position.y, 0.0f}, rotation, size, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, float rotation, const glm::vec2& size,
	                                 const glm::vec4& color)
	{
		HZ_PROFILE_FUNC();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;


		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
			* glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[quadVertexCount] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
		};
		for (int i = 0; i < quadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, float rotation, const glm::vec2& size,
	                                 const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({position.x, position.y, 0.0f}, rotation, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, float rotation, const glm::vec2& size,
	                                 const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNC()

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			FlushAndReset();
		}

		float textureIndex = 0.0f;
		for (int i = 1; i < s_Data.TextureSlotIndex; ++i)
		{
			if (*s_Data.TextureSlots[i].Raw() == *texture.Raw())
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
			* glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

		const glm::vec4& color = tintColor;

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[quadVertexCount] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
		};
		for (int i = 0; i < quadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& spriteComp, int entityID)
	{
		DrawQuad(transform, spriteComp.Color, entityID);
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}
}

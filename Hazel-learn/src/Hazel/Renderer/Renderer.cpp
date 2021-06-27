#include "hazelPCH.h"

#include "Renderer.h"


#include "Renderer2D.h"
#include "Platform/OpenGL/OpenGLRenderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {
	struct RendererData
	{
		Ref<ShaderLibrary> m_ShaderLibrary;
		Ref<Texture2D> WhiteTexture;
	};

	struct ShaderDependencies
	{
		std::vector<Ref<Pipeline>> Pipelines;
		std::vector<Ref<Material>> Materials;
	};

	static RendererAPI* InitRendererAPI()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::OpenGL: return new OpenGLRenderer();
		}
		HZ_CORE_ASSERT(false, "Unknow Renderer API type");
		return nullptr;
	}

	static RendererData* s_Data = nullptr;
	static RenderCommandQueue* s_CommandQueue = nullptr;
	static RenderCommandQueue s_ResourceFreeQueue[3];
	static RendererAPI* s_RendererAPI = nullptr;
	static std::unordered_map<size_t, Ref<Pipeline>> s_PipelineCache;
	static std::unordered_map<size_t, ShaderDependencies> s_ShaderDependencies;

	void Renderer::Init()
	{
		s_Data = new RendererData();
		s_Data->m_ShaderLibrary = Ref<ShaderLibrary>::Create();
		s_CommandQueue = new RenderCommandQueue();
		s_RendererAPI = InitRendererAPI();

		//init load shader
		s_Data->m_ShaderLibrary->Load("Resources/Shaders/Renderer2D.glsl");
		//compile shaders
		WaitAndRender();

		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture = Texture2D::Create(ImageFormat::RGBA, 1, 1, &whiteTextureData);
		
		s_RendererAPI->Init();
		Renderer2D::Init();
	}

	void Renderer::ShutDown()
	{
		Renderer2D::Shutdown();
		s_ShaderDependencies.clear();
		s_RendererAPI->Shutdown();

		delete s_Data;
		delete s_CommandQueue;
	}

	RendererCapabilities& Renderer::GetCapabilities()
	{
		return s_RendererAPI->GetCapabilities();
	}

	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return s_Data->m_ShaderLibrary;
	}

	void Renderer::WaitAndRender()
	{
		s_CommandQueue->Execute();
	}

	void Renderer::BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<RenderPass> renderPass, bool explicitClear)
	{
		s_RendererAPI->BeginRenderPass(renderCommandBuffer, renderPass, explicitClear);
	}

	void Renderer::EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer)
	{
		s_RendererAPI->EndRenderPass(renderCommandBuffer);
	}

	void Renderer::BeginFrame()
	{
		s_RendererAPI->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		s_RendererAPI->EndFrame();
	}

	void Renderer::RenderQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipline, Ref<UniformBufferSet> uniformBufferSet,
		Ref<Material> material, const glm::mat4& transform)
	{
		s_RendererAPI->RenderQuad(renderCommandBuffer, pipline, uniformBufferSet, material, transform);
	}

	void Renderer::RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline,
		Ref<UniformBufferSet> uniformBufferSet, Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer,
		const glm::mat4& transform, uint32_t indexCount)
	{
		s_RendererAPI->RenderGeometry(renderCommandBuffer, pipeline, uniformBufferSet, material, vertexBuffer, indexBuffer, transform, indexCount);
	}

	void Renderer::SubmitQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Material> material, const glm::mat4& transform)
	{
		//empty
	}

	Ref<Texture2D> Renderer::GetWhiteTexture()
	{
		return s_Data->WhiteTexture;
	}

	uint32_t Renderer::GetCurrentFrameIndex()
	{
		//TODO
		return 0;
	}

	RendererConfig& Renderer::GetConfig()
	{
		static RendererConfig config;
		return config;
	}

	RenderCommandQueue& Renderer::GetRenderCommandQueue()
	{
		return *s_CommandQueue;
	}
}

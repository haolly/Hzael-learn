#include "hazelPCH.h"
#include "OpenGLRenderer.h"

#include "OpenGLMaterial.h"
#include "OpenGLShader.h"
#include "glad/glad.h"
#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/RenderPass.h"
#include "Hazel/Renderer/RendererCapabilities.h"

namespace Hazel
{
	struct OpenGLRendererData
	{
		RendererCapabilities RenderCaps;

		Ref<VertexBuffer> m_FullscreenQuadVertexBuffer;
		Ref<IndexBuffer> m_FullscreenQuadIndexBuffer;

		PipelineSpecification m_FullscreenPipelineSpec;
		Ref<RenderPass> ActiveRenderPass;

		Ref<Texture2D> BRDFLut;
	};

	static OpenGLRendererData* s_Data = nullptr;

	namespace Utils
	{
		static void Clear(float r, float g, float b, float a)
		{
			glClearColor(r, g, b, a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		static void SetClearColor(float r, float g, float b, float a)
		{
			glClearColor(r, g, b, a);
		}

		static void SetLineThickness(float thickness)
		{
			glLineWidth(thickness);
		}

		static void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
		                             const void* userParam)
		{
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH:
				{
					HZ_CORE_ERROR("[OpenGL Debug HIGH] {0}", message);
					HZ_CORE_ASSERT(false, "[OpenGL Debug HIGH]");
					break;
				}
			case GL_DEBUG_SEVERITY_MEDIUM:
				{
					HZ_CORE_WARN("[OpenGL Debug Medium] {0}", message);
					break;
				}
				
			case GL_DEBUG_SEVERITY_LOW:
				{
					HZ_CORE_INFO("[OpenGL Debug Low] {0}", message);
					break;
				}
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				break;
			}
		}
	}

	void OpenGLRenderer::Init()
	{
		s_Data = new OpenGLRendererData();
		Renderer::Submit([]()
		{
			glDebugMessageCallback(Utils::OpenGLLogMessage, nullptr);
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			auto& caps = s_Data->RenderCaps;
			caps.Vendor = (const char*)glGetString(GL_VENDOR);
			caps.Device = (const char*)glGetString(GL_RENDERER);
			caps.Version = (const char*)glGetString(GL_VERSION);
			HZ_CORE_TRACE("OpenGLRendererData::Init");
			Utils::DumpGPUInfo();

			//TODO, WHY NOT USE PIPELINE?
			unsigned int vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glEnable(GL_DEPTH_TEST);
			glFrontFace(GL_CCW);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_MULTISAMPLE);
			glEnable(GL_STENCIL_TEST);

			glGetIntegerv(GL_MAX_SAMPLES, &caps.MaxSamples);
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &caps.MaxAnisotropy);

			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &caps.MaxTextureUnits);

			GLenum error = glGetError();
			while (error != GL_NO_ERROR)
			{
				HZ_CORE_ERROR("OpenGL Error : {0}", error);
				error = glGetError();
			}
		});

		// create fullscreen quad
	}

	void OpenGLRenderer::Shutdown()
	{
		OpenGLShader::ClearUniformBuffers();
		delete s_Data;
	}

	void OpenGLRenderer::BeginFrame()
	{
	}

	void OpenGLRenderer::EndFrame()
	{
	}

	void OpenGLRenderer::BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, const Ref<RenderPass>& renderPass,
	                                     bool explicitClear)
	{
		s_Data->ActiveRenderPass = renderPass;
		renderPass->GetSpecification().TargetFramebuffer->Bind();
		if(explicitClear)
		{
			const glm::vec4& clearColor = renderPass->GetSpecification().TargetFramebuffer->GetSpecification().ClearColor;
			Renderer::Submit([=]()
			{
				Utils::Clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
			});
		}
	}

	void OpenGLRenderer::EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer)
	{
		s_Data->ActiveRenderPass = nullptr;
	}

	RendererCapabilities& OpenGLRenderer::GetCapabilities()
	{
		return s_Data->RenderCaps;
	}

	void OpenGLRenderer::SubmitFullscreenQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline,
		Ref<UniformBufferSet> uniformBufferSet, Ref<Material> material)
	{
		auto shader = material->GetShader();
		bool depthTest = true;
		Ref<OpenGLMaterial> glMaterial = material.As<OpenGLMaterial>();
		if(material)
		{
			glMaterial->UpdateForRendering();
			depthTest = material->GetFlag(MaterialFlag::DepthTest);
		}

		pipeline->Bind();
		s_Data->m_FullscreenQuadVertexBuffer->Bind();
		s_Data->m_FullscreenQuadIndexBuffer->Bind();
		Renderer::Submit([depthTest]()
		{
			if(!depthTest)
				glDisable(GL_DEPTH_TEST);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			if(!depthTest)
				glEnable(GL_DEPTH_TEST);
		});
	}

	void OpenGLRenderer::RenderQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline,
		Ref<UniformBufferSet> uniformBufferSet, Ref<Material> material, const glm::mat4& transform)
	{
		pipeline->Bind();
		s_Data->m_FullscreenQuadVertexBuffer->Bind();
		s_Data->m_FullscreenQuadIndexBuffer->Bind();
		Ref<OpenGLMaterial> glMaterial = material.As<OpenGLMaterial>();
		glMaterial->UpdateForRendering();

		auto shader = material->GetShader().As<OpenGLShader>();
		HZ_CORE_ASSERT(shader == pipeline->GetSpecification().Shader, "");
		shader->SetUniform("u_Renderer.Transform", transform);

		Renderer::Submit([material]()
		{
			if(material->GetFlag(MaterialFlag::DepthTest))
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			glDrawElements(GL_TRIANGLES, s_Data->m_FullscreenQuadIndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
		});
	}

	void OpenGLRenderer::RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline,
		Ref<UniformBufferSet> uniformBufferSet, Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer,
		const glm::mat4& transform, uint32_t indexCount)
	{
		//todo, empty
	}
}

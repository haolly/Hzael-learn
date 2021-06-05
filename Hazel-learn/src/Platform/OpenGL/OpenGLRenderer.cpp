#include "hazelPCH.h"
#include "OpenGLRenderer.h"

#include "glad/glad.h"
#include "Hazel/Renderer/Renderer.h"
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
		});
	}

	void OpenGLRenderer::Shutdown()
	{
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
	}

	void OpenGLRenderer::EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer)
	{
	}

	RendererCapabilities& OpenGLRenderer::GetCapabilities()
	{
	}
}

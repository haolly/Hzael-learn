#include "hazelPCH.h"
#include "Framebuffer.h"
#include "Hazel/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"


namespace Hazel
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
			{
				HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				return Ref<OpenGLFramebuffer>::Create(spec);
			}
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;
	}
}


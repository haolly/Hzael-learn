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
			case RendererAPIType::None:
			{
				HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			}
			case RendererAPIType::OpenGL:
			{
				return Ref<OpenGLFramebuffer>::Create(spec);
			}
		case RendererAPIType::Vulkan: break;
		default: ;
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;
	}

	FramebufferPool* FramebufferPool::s_Instance = new FramebufferPool();
	FramebufferPool::FramebufferPool(uint32_t maxFramebuffers)
	{
	}

	FramebufferPool::~FramebufferPool()
	{
	}

	std::weak_ptr<Framebuffer> FramebufferPool::AllocateBuffer()
	{
		return std::weak_ptr<Framebuffer>();
	}

	void FramebufferPool::Add(const Ref<Framebuffer>& framebuffer)
	{
		m_Pool.push_back(framebuffer);
	}
}


#include "hazelPCH.h"
#include "RenderPass.h"

#include "Renderer.h"

namespace Hazel
{
	class OpenGLRenderPass;

	Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPIType::None: break;
		case RendererAPIType::OpenGL: return Ref<OpenGLRenderPass>::Create(spc);
		case RendererAPIType::Vulkan: break;
		}
		HZ_CORE_ASSERT(false, "Unknow renderer api type");
		return nullptr;
	}
}

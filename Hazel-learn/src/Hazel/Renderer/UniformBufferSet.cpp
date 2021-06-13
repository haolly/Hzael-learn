#include "hazelPCH.h"
#include "UniformBufferSet.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLUniformBufferSet.h"

namespace Hazel
{
	Ref<UniformBufferSet> UniformBufferSet::Create(uint32_t frames)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::OpenGL: return Ref<OpenGLUniformBufferSet>::Create(frames);
		case RendererAPIType::Vulkan: break;
		}
		HZ_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
}

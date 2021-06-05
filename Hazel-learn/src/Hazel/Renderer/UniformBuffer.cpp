#include "hazelPCH.h"
#include "UniformBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Hazel
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPIType::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported");
			return nullptr;
		case RendererAPIType::OpenGL:
			return Ref<OpenGLUniformBuffer>::Create(size, binding);
		}
		HZ_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
}

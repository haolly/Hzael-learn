#include "hazelPCH.h"
#include "RenderCommandBuffer.h"

#include "RendererAPI.h"

namespace Hazel
{
	Ref<RenderCommandBuffer> RenderCommandBuffer::Create(uint32_t count, const std::string& debugName)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPIType::None:
				return nullptr;
			case RendererAPIType::OpenGL:
				return nullptr;
		}
		//todo
		return nullptr;
	}

	Ref<RenderCommandBuffer> RenderCommandBuffer::CreateFromSwapChain(const std::string& debugName)
	{
		return nullptr;
	}
}

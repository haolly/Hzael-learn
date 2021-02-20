#include "hazelPCH.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Hazel 
{
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:
			{
				HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			}
			case RendererAPI::OpenGL:
			{
				return new OpenGLIndexBuffer(indices, count);
			}
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;
	}
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:
			{
				HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			}
			case RendererAPI::OpenGL:
			{
				return new OpenGLVertexBuffer(vertices, size);
			}
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;
	}
}
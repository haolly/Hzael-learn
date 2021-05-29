#include "hazelPCH.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Hazel 
{

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
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
				return Ref<OpenGLIndexBuffer>::Create(indices, count);
			}
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
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
				return Ref<OpenGLVertexBuffer>::Create(size);
			}
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;

	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
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
				return Ref<OpenGLVertexBuffer>::Create(vertices, size);
			}
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;
	}
}
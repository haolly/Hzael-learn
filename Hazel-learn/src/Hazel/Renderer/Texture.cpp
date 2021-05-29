#include "hazelPCH.h"
#include "Texture.h"

#include "Hazel/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Hazel {
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
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
				return Ref<OpenGLTexture2D>::Create(width, height);
			}
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;

	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
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
				return Ref<OpenGLTexture2D>::Create(path);
			}
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;
    }
}

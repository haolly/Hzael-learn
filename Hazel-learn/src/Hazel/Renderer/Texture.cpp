#include "hazelPCH.h"
#include "Texture.h"

#include "Hazel/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Hazel {
	Ref<Texture2D> Texture2D::Create(ImageFormat format, uint32_t width, uint32_t height, const void* data, TextureProperties properties)
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
				return Ref<OpenGLTexture2D>::Create(format, width, height, data, properties);
			}
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;

	}

	Ref<Texture2D> Texture2D::Create(const std::string& path, TextureProperties properties)
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
				return Ref<OpenGLTexture2D>::Create(path, properties);
			}
		}
		HZ_CORE_ASSERT(false, "RendererAPI is not set");
		return nullptr;
    }
}

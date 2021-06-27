#include "hazelPCH.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "OpenGLImage2D.h"
#include "Hazel/Renderer/Renderer.h"

namespace Hazel
{
	OpenGLTexture2D::OpenGLTexture2D(ImageFormat format, uint32_t width, uint32_t height, const void* data, TextureProperties properties)
		: m_Properties(properties), m_Width(width), m_Height(height)
	{
		ImageSpecification spec;
		spec.Format = format;
		spec.Width = width;
		spec.Height = height;
		m_Image = Image2D::Create(spec, data);
		m_Image.As<OpenGLImage2D>()->CreateSampler(m_Properties);
		Renderer::Submit([=]()
		{
			m_Image->Invalidate();
		});
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, TextureProperties properties)
		: m_Properties(properties), m_Path(path)
	{
		int width, height, channel;
		if(stbi_is_hdr(path.c_str()))
		{
			HZ_CORE_INFO("Load HDR texture {0}, srgb={1}", path, properties.SRGB);
			float* imageData = stbi_loadf(path.c_str(), &width, &height, &channel, STBI_rgb_alpha);
			HZ_CORE_ASSERT(imageData, "");
			Buffer buffer(imageData, Utils::GetImageMemorySize(ImageFormat::RGBA32F, width, height));
			ImageSpecification spec;
			spec.Format = ImageFormat::RGBA32F;
			spec.Width = width;
			spec.Height = height;
			m_Image = Image2D::Create(spec, buffer);
		}
		else
		{
			HZ_CORE_INFO("Loading texture {0}, srgb={1}", path, properties.SRGB);
			stbi_uc* imageData = stbi_load(path.c_str(), &width, &height, &channel, 0);
			HZ_CORE_ASSERT(imageData, "Failed to load image");
			ImageFormat format = properties.SRGB ? ImageFormat::RGB : ImageFormat::RGBA;
			Buffer buffer(imageData, Utils::GetImageMemorySize(format, width, height));
			ImageSpecification spec;
			spec.Format = format;
			spec.Width = width;
			spec.Height = height;
			m_Image = Image2D::Create(spec, buffer);
		}
		m_Image.As<OpenGLImage2D>()->CreateSampler(m_Properties);
		m_Width = width;
		m_Height = height;
		m_Loaded = true;

		Ref<Image2D>& image = m_Image;
		Renderer::Submit([image]() mutable
		{
			image->Invalidate();
			Buffer& buffer = image->GetBuffer();
			stbi_image_free(buffer.Data);
			buffer = Buffer();
		});
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		Ref<Image2D> image = m_Image;
		Renderer::Submit([image]() mutable
		{
			image->Release();
		});
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		Ref<OpenGLImage2D> image = m_Image.As<OpenGLImage2D>();
		Renderer::Submit([slot, image]()
		{
			glBindTextureUnit(slot, image->GetRendererID());
		});
	}

	uint32_t OpenGLTexture2D::GetMipLevelCount() const
	{
		return Utils::CalculateMipCount(m_Width, m_Height);
	}

	void OpenGLTexture2D::Lock()
	{
		m_Locked = true;
	}

	// lock --> write --> unlock
	void OpenGLTexture2D::Unlock()
	{
		m_Locked = false;
		Ref<OpenGLTexture2D> instance = this;
		Ref<OpenGLImage2D> image = m_Image.As<OpenGLImage2D>();
		// Mutable allows lambda to modify captured parameter
		Renderer::Submit([instance, image]() mutable
		{
			glTextureSubImage2D(image->GetRendererID(), 0, 0, 0, instance->m_Width, instance->m_Height, Utils::OpenGLImageFormat(image->GetSpecification().Format),
				GL_UNSIGNED_BYTE, instance->m_Image->GetBuffer().Data);
		});
	}

	Buffer OpenGLTexture2D::GetWritableBuffer()
	{
		HZ_CORE_ASSERT(m_Locked, "Texture must be locked to get writable buffer");
		return m_Image->GetBuffer();
	}
}

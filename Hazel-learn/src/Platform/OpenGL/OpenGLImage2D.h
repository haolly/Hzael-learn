#pragma once
#include "glad/glad.h"
#include "Hazel/Renderer/Image.h"

namespace Hazel
{
	class OpenGLImage2D : public Image2D
	{
	public:
		OpenGLImage2D(ImageSpecification specification, Buffer buffer);
		OpenGLImage2D(ImageSpecification specification, const void* data = nullptr);
		~OpenGLImage2D() override;
		void Invalidate() override;
		void Release() override;
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		ImageSpecification& GetSpecification() override;
		const ImageSpecification& GetSpecification() const override;
		Buffer GetBuffer() const override;
		Buffer& GetBuffer() override;
		void CreatePerLayerImageViews() override;

		uint32_t& GetRendererID() { return m_RendererID; }
		uint32_t GetRendererID() const { return m_RendererID; }
		uint32_t& GetSamplerRendererID() { return m_SamplerRendererID; }
		uint32_t GetSamplerRendererID() const { return m_SamplerRendererID; }
		
		void CreateSampler(TextureProperties properties);
		uint64_t GetHash() const override;
	private:
		ImageSpecification m_Specification;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID = 0;
		uint32_t m_SamplerRendererID = 0;

		Buffer m_ImageData;
	};

	namespace Utils
	{
		inline GLenum OpenGLImageFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:
				case ImageFormat::SRGB:
					return GL_RGB;
				
				case ImageFormat::RGBA:
				case ImageFormat::RGBA16F:
				case ImageFormat::RGBA32F:
					return GL_RGBA;
			}
			
			HZ_CORE_ASSERT(false, "Unknow Image format");
			return 0;
		}
		
		inline GLenum OpenGLImageInternalFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:
					return GL_RGB;
				
				case ImageFormat::SRGB:
					return GL_SRGB;
				
				case ImageFormat::RGBA:
					return GL_RGBA8;
				
				case ImageFormat::RGBA16F:
					return GL_RGBA16F;
				
				case ImageFormat::RGBA32F:
					return GL_RGBA32F;
				
				case ImageFormat::DEPTH24STENCIL8:
					return GL_DEPTH24_STENCIL8;
				
				case ImageFormat::DEPTH32F:
					return GL_DEPTH_COMPONENT32F;
			}
			HZ_CORE_ASSERT(false, "Unknow Image format");
			return 0;
		}

		inline GLenum OpenGLFormatDataType(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:
				case ImageFormat::SRGB:
				case ImageFormat::RGBA:
					return GL_UNSIGNED_BYTE;
				case ImageFormat::RGBA16F:
				case ImageFormat::RGBA32F:
					return GL_FLOAT;
			}
			HZ_CORE_ASSERT(false, "Unknow Image format");
			return 0;
		}

		inline GLenum OpenGLSamplerWrap(TextureWrap wrap)
		{
			switch (wrap)
			{
				case TextureWrap::Clamp:
					return GL_CLAMP_TO_EDGE;
				case TextureWrap::Repeat:
					return GL_REPEAT;
			}
			HZ_CORE_ASSERT(false, "Unknow wrap mode");
			return 0;
		}

		inline GLenum OpenGLSamplerFilter(TextureFilter filter, bool mipmap)
		{
			switch (filter)
			{
				case TextureFilter::Linear:
					return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
				
				case TextureFilter::Nearest:
					return mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
			}
			HZ_CORE_ASSERT(false, "Unknow Filter mode");
			return 0;
		}
	}
}

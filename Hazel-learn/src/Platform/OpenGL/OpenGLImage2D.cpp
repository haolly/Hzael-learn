#include "hazelPCH.h"
#include "OpenGLImage2D.h"
#include <glad/glad.h>

#include "../../../vendor/yaml-cpp/src/emitterutils.h"
#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Utils/StringUtils.h"

namespace Hazel
{
	OpenGLImage2D::OpenGLImage2D(ImageSpecification specification, Buffer buffer)
		: m_Specification(specification), m_Width(specification.Width), m_Height(specification.Height), m_ImageData(buffer)
	{
	}

	OpenGLImage2D::OpenGLImage2D(ImageSpecification specification, const void* data)
		: m_Specification(specification), m_Width(specification.Width), m_Height(specification.Height)
	{
		if(data)
			m_ImageData = Buffer::Copy(data, Utils::GetImageMemorySize(specification.Format, specification.Width, specification.Height));
	}

	OpenGLImage2D::~OpenGLImage2D()
	{
		m_ImageData.Release();
		if(m_RendererID)
		{
			uint32_t rendererID = m_RendererID;
			Renderer::Submit([rendererID]()
			{
				glDeleteTextures(1, &rendererID);
			});
		}
	}

	void OpenGLImage2D::Invalidate()
	{
		if(m_RendererID)
			Release();

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		GLenum internalFormat = Utils::OpenGLImageInternalFormat(m_Specification.Format);
		uint32_t mipCount = Utils::CalculateMipCount(m_Width, m_Height);
		// Immutable storage,
		// ref https://www.khronos.org/opengl/wiki/Texture_Storage
		glTextureStorage2D(m_RendererID, mipCount, internalFormat, m_Width, m_Height);
		if(m_ImageData)
		{
			GLenum format = Utils::OpenGLImageFormat(m_Specification.Format);
			GLenum dataType = Utils::OpenGLFormatDataType(m_Specification.Format);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, format, dataType, m_ImageData.Data);
			glGenerateTextureMipmap(m_RendererID);
		}
	}

	void OpenGLImage2D::Release()
	{
		if(m_RendererID)
		{
			glDeleteTextures(1, &m_RendererID);
			m_RendererID = 0;
		}
		m_ImageData.Release();
	}

	uint32_t OpenGLImage2D::GetWidth() const
	{
		return m_Width;
	}

	uint32_t OpenGLImage2D::GetHeight() const
	{
		return m_Height;
	}

	ImageSpecification& OpenGLImage2D::GetSpecification()
	{
		return m_Specification;
	}

	const ImageSpecification& OpenGLImage2D::GetSpecification() const
	{
		return m_Specification;
	}

	Buffer OpenGLImage2D::GetBuffer() const
	{
		return m_ImageData;
	}

	Buffer& OpenGLImage2D::GetBuffer()
	{
		return m_ImageData;
	}

	void OpenGLImage2D::CreatePerLayerImageViews()
	{
	}

	uint64_t OpenGLImage2D::GetHash() const
	{
		return (uint64_t)m_RendererID;
	}

	void OpenGLImage2D::CreateSampler(TextureProperties properties)
	{
		glCreateSamplers(1, &m_SamplerRendererID);
		glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_MIN_FILTER, Utils::OpenGLSamplerFilter(properties.SamplerFilter, properties.GenerateMips));
		glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_MAG_FILTER, Utils::OpenGLSamplerFilter(properties.SamplerFilter, false));
		glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_WRAP_R, Utils::OpenGLSamplerWrap(properties.SamplerWrap));
		glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_WRAP_S, Utils::OpenGLSamplerWrap(properties.SamplerWrap));
		glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_WRAP_T, Utils::OpenGLSamplerWrap(properties.SamplerWrap));
	}
}

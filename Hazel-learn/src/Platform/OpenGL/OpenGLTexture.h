#pragma once
#include "glad/glad.h"
#include "Hazel/Renderer/Texture.h"

namespace Hazel
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		void Bind(uint32_t slot=0) const override;
		void SetData(void* data, uint32_t size) override;
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID{};
		GLenum m_InternalFormat, m_DataFormat;
	};
}


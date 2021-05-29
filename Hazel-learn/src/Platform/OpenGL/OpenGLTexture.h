#pragma once
#include "glad/glad.h"
#include "Hazel/Renderer/Texture.h"

namespace Hazel
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		explicit OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		void Bind(uint32_t slot=0) const override;
		void SetData(void* data, uint32_t size) override;
		bool operator==(const Texture& other) const override
		{
			return m_RendererID == dynamic_cast<const OpenGLTexture2D&>(other).m_RendererID;
		}

		uint32_t GetRendererID() const override
		{
			return m_RendererID;
		}

		bool Loaded() const override { return m_Loaded;}
	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID{};
		GLenum m_InternalFormat, m_DataFormat;

		bool m_Loaded = false;
	};
}


#pragma once
#include "glad/glad.h"
#include "Hazel/Renderer/Texture.h"

namespace Hazel
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(ImageFormat format, uint32_t width, uint32_t height, const void* data, TextureProperties properties);
		OpenGLTexture2D(const std::string& path, TextureProperties properties);
		virtual ~OpenGLTexture2D();

		virtual void Bind(uint32_t slot=0) const;
		
		Ref<Image2D> GetImage() const override { return m_Image;}
		uint32_t GetWidth() const override { return m_Width;}
		uint32_t GetHeight() const override { return m_Height;}

		bool Loaded() const override { return m_Loaded;}
		ImageFormat GetFormat() const override { return m_Image->GetSpecification().Format;}
		uint32_t GetMipLevelCount() const override;
		uint64_t GetHash() const override {return m_Image->GetHash(); }
		void Lock() override;
		void Unlock() override;
		Buffer GetWritableBuffer() override;
		const std::string& GetPath() const override { return m_Path;}
	private:
		Ref<Image2D> m_Image;
		TextureProperties m_Properties;
		std::string m_Path;
		uint32_t m_Width, m_Height;
		
		bool m_Locked = false;
		bool m_Loaded = false;
		bool m_IsHDR = false;
	};
}


#pragma once
#include "Image.h"
#include "Hazel/Asset/Asset.h"

namespace Hazel {
	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;
		
		virtual ImageFormat GetFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetMipLevelCount() const = 0;

		virtual uint64_t GetHash() const = 0;
		virtual TextureType GetType() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(ImageFormat format, uint32_t width, uint32_t height, const void* data = nullptr, TextureProperties properties = TextureProperties());
		static Ref<Texture2D> Create(const std::string& path, TextureProperties properties = TextureProperties());

		virtual Ref<Image2D> GetImage() const = 0;
		virtual void Lock() = 0;
		virtual void Unlock() = 0;

		virtual Buffer GetWritableBuffer() = 0;
		
		virtual bool Loaded() const = 0;

		virtual const std::string& GetPath() const = 0;
		virtual TextureType GetType() const override { return TextureType::Texture2D; }
		
		static AssetType GetStaticType() { return AssetType::Texture;}
		virtual AssetType GetAssetType() const override { return GetStaticType();}
	};
}


#include "hazelPCH.h"
#include "AssetSerializer.h"

#include "Hazel/Renderer/Texture.h"

namespace Hazel
{
	bool TextureSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		asset = Texture2D::Create(metadata.FilePath);
		asset->Handle = metadata.Handle;

		bool result = asset.As<Texture2D>()->Loaded();
		if(!result)
			asset->SetFlag(AssetFlag::Invalid, true);

		return result;
	}

	bool MeshAssetSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		//todo
		return false;
	}

	bool EnvironmentSerializer::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset) const
	{
		//todo
		return false;
	}
}

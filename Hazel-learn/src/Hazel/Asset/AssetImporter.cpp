#include "hazelPCH.h"
#include "AssetImporter.h"

namespace Hazel
{
	void AssetImporter::Init()
	{
		s_Serializers[AssetType::Texture] = CreateScope<TextureSerializer>();
		s_Serializers[AssetType::MeshAsset] = CreateScope<MeshAssetSerializer>();
		s_Serializers[AssetType::EnvMap] = CreateScope<EnvironmentSerializer>();
	}

	void AssetImporter::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		if(s_Serializers.find(metadata.Type) == s_Serializers.end())
		{
			HZ_CORE_WARN("There is currently no importer for asset of type: {0}", metadata.Type);
			return;
		}
		s_Serializers[metadata.Type]->Serialize(metadata, asset);
	}

	bool AssetImporter::TryLoadData(const AssetMetadata& metadata, Ref<Asset>& asset)
	{
		if(s_Serializers.find(metadata.Type) == s_Serializers.end())
		{
			HZ_CORE_WARN("There is currently no importer for asset of type: {0}", metadata.Type);
			return false;
		}
		return s_Serializers[metadata.Type]->TryLoadData(metadata, asset);
	}
	
	std::unordered_map<AssetType, Scope<AssetSerializer>> AssetImporter::s_Serializers;
}

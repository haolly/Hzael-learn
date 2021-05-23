#pragma once
#include "Hazel/Core/Base.h"
namespace Hazel
{
	enum class AssetFlag : uint16_t
	{
		None = 0,
		Missing = BIT(0),
		Invalid = BIT(1),
	};

	enum class AssetType : uint16_t
	{
		None = 0,
		Scene = 1,
		MeshAsset = 2,
		//todo, what's the difference with MeshAsset
		Mesh = 3,
		Texture = 4,
		EnvMap = 5,
		Audio = 6,
		PhysicsMat = 7,
	};

	namespace Utils
	{
		inline AssetType AssetTypeFromString(const std::string& assetType)
		{
			if(assetType == "None") return AssetType::None;
			if(assetType == "Scene") return AssetType::Scene;
			if(assetType == "MeshAsset") return AssetType::MeshAsset;
			if(assetType == "Mesh") return AssetType::Mesh;
			if(assetType == "Texture") return AssetType::Texture;
			if(assetType == "EnvMap") return AssetType::EnvMap;
			if(assetType == "Audio") return AssetType::Audio;
			if(assetType == "PhysicsMap") return AssetType::PhysicsMat;

			HZ_CORE_ASSERT(false, "Unknow Asset Type");
			return AssetType::None;
		}

		inline const char* AssetTypeToString(AssetType assetType)
		{
			switch (assetType)
			{
			case AssetType::None: return "None";
			case AssetType::Scene: return "Scene";
			case AssetType::MeshAsset: return "MeshAsset";
			case AssetType::Mesh: return "Mesh";
			case AssetType::Texture: return "Texture";
			case AssetType::EnvMap: return "EnvMap";
			case AssetType::Audio: return "Audio";
			case AssetType::PhysicsMat: return "PhysicsMat";
			default: HZ_CORE_ASSERT(false, "Unknow Asset Type");
			}
			return "None";
		}
	}
}

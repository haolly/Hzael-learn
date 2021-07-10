#pragma once

#include <unordered_map>
#include "AssetTypes.h"

namespace Hazel
{
	inline static std::unordered_map<std::string, AssetType> s_AssetExtensionMap = {
		{".hscene", AssetType::Scene},
		{".hmesh", AssetType::Mesh},
		{".fbx", AssetType::MeshAsset},
		{".png", AssetType::Texture},
		{".jpg", AssetType::Texture},
		{".jpeg", AssetType::Texture},
	};
}

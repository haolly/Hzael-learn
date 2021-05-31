﻿#pragma once
#include "Asset.h"

namespace Hazel
{
	struct AssetMetadata
	{
		AssetHandle Handle = 0;
		AssetType Type;

		std::string FilePath;
		std::string FileName;
		std::string Extension;
		bool IsDataLoaded = false;

		bool IsValid() const { return Handle != 0;}
	};
}

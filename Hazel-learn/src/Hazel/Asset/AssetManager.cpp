#include "hazelPCH.h"
#include "AssetManager.h"
#include "Hazel/Utils/StringUtils.h"

#include <filesystem>


#include "AssetExtensions.h"
#include "AssetImporter.h"
#include "yaml-cpp/emitter.h"

namespace Hazel
{
	static const char* s_AssetRegistryPath = "assets/AssetRegistry.hzr";
	//TODO, temporary
	static AssetMetadata s_NullMetadata;

	void AssetManager::Init()
	{
		AssetImporter::Init();

		LoadAssetRegistry();
		//FileSystem::SetChangeCallback(AssetManager::OnFileSystemChanged);
		ReloadAssets();
		WriteRegistryToFile();
	}

	// void AssetManager::SetAssetChangeCallback(const AssetsChangeEventFn& callback)
	// {
	// 	s_AssetsChangeCallback = callback;
	// }

	void AssetManager::Shutdown()
	{
		WriteRegistryToFile();

		s_AssetRegistry.clear();
		s_LoadedAssets.clear();
	}

	AssetMetadata& AssetManager::GetMetadata(AssetHandle handle)
	{
		for (auto& [filepath, metadata] : s_AssetRegistry)
		{
			if(metadata.Handle == handle)
				return metadata;
		}
		return s_NullMetadata;
	}

	AssetMetadata& AssetManager::GetMetadata(const std::string& filepath)
	{
		std::string fixedFilePath = filepath;
		std::replace(fixedFilePath.begin(), fixedFilePath.end(), '\\', '/');
		if(s_AssetRegistry.find(fixedFilePath) != s_AssetRegistry.end())
			return s_AssetRegistry[fixedFilePath];
		return s_NullMetadata;
	}

	AssetHandle AssetManager::GetAssetHandleFromFilePath(const std::string& filepath)
	{
		std::string fixedFilePath = filepath;
		std::replace(fixedFilePath.begin(), fixedFilePath.end(), '\\', '/');
		if(s_AssetRegistry.find(fixedFilePath) != s_AssetRegistry.end())
			return s_AssetRegistry[fixedFilePath].Handle;
		return 0;
	}

	void AssetManager::Rename(AssetHandle assetHandle, const std::string& newName)
	{
	}

	void AssetManager::RemoveAsset(AssetHandle assetHandle)
	{
		AssetMetadata metdata = GetMetadata(assetHandle);
		s_AssetRegistry.erase(metdata.FilePath);
		s_LoadedAssets.erase(metdata.Handle);

		WriteRegistryToFile();
	}

	AssetType AssetManager::GetAssetTypeFromExtension(const std::string& extension)
	{
		std::string ext = Utils::StringUtils::ToLower(extension);
		if(s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
			return AssetType::None;

		return s_AssetExtensionMap.at(extension.c_str());
	}

	AssetType AssetManager::GetAssetTypeFromPath(const std::filesystem::path& path)
	{
		return GetAssetTypeFromExtension(path.extension().string());
	}

	AssetHandle AssetManager::ImportAsset(const std::string& filepath)
	{
		std::string fixedFilePath = filepath;
		std::replace(fixedFilePath.begin(), fixedFilePath.end(), '\\', '/');

		if(s_AssetRegistry.find(fixedFilePath) != s_AssetRegistry.end())
			return 0;

		AssetType type = GetAssetTypeFromExtension(Utils::StringUtils::GetExtension(fixedFilePath));
		//TODO: Improve this 
		if(type == AssetType::None)
			return 0;

		AssetMetadata metadata;
		metadata.Handle = AssetHandle();
		metadata.FilePath = fixedFilePath;
		metadata.FileName = Utils::StringUtils::RemoveExtension(Utils::StringUtils::GetFileName(fixedFilePath));
		metadata.Extension = Utils::StringUtils::GetExtension(fixedFilePath);
		metadata.Type = type;
		s_AssetRegistry[fixedFilePath] = metadata;

		return metadata.Handle;
	}

	bool AssetManager::ReloadData(AssetHandle assetHandle)
	{
		auto& metadata = GetMetadata(assetHandle);
		if(!metadata.IsDataLoaded)
		{
			HZ_CORE_WARN("Trying to reload asset that was never loaded");

			Ref<Asset> asset;
			metadata.IsDataLoaded = AssetImporter::TryLoadData(metadata, asset);
			return metadata.IsDataLoaded;
		}
		HZ_CORE_ASSERT(s_LoadedAssets.find(assetHandle) != s_LoadedAssets.end());
		Ref<Asset>& asset = s_LoadedAssets.at(assetHandle);
		metadata.IsDataLoaded = AssetImporter::TryLoadData(metadata, asset);
		return metadata.IsDataLoaded;
	}

	void AssetManager::LoadAssetRegistry()
	{
	}

	void AssetManager::ProcessDirectory(const std::string& directoryPath)
	{
		for(auto entry: std::filesystem::directory_iterator(directoryPath))
		{
			if(entry.is_directory())
				ProcessDirectory(entry.path().string());
			else
				ImportAsset(entry.path().string());
		}
	}

	void AssetManager::ReloadAssets()
	{
		ProcessDirectory("assets");
		WriteRegistryToFile();
	}

	void AssetManager::WriteRegistryToFile()
	{
		YAML::Emitter out;
		out<<YAML::BeginMap;

		out<<YAML::Key << "Assets" << YAML::BeginSeq;
		for(auto& [filepath, metdata] : s_AssetRegistry)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Handle" << YAML::Value << metdata.Handle;
			out << YAML::Key << "FilePath" << YAML::Value << metdata.FilePath;
			out << YAML::Key << "Type" << YAML::Value << Utils::AssetTypeToString(metdata.Type);
			out << YAML::EndMap;
		}
		out<<YAML::EndSeq;
		out<<YAML::EndMap;

		std::ofstream fout(s_AssetRegistryPath);
		fout << out.c_str();
		fout.close();
	}

	void AssetManager::OnAssetRenamed(AssetHandle assetHandle, const std::string& newFilePath)
	{
	}

	std::unordered_map<AssetHandle, Ref<Asset>> AssetManager::s_LoadedAssets;
	// store imported asset
	std::unordered_map<std::string, AssetMetadata> AssetManager::s_AssetRegistry;
	//AssetsChangeEventFn AssetManager::s_AssetsChangeCallback;
	
}

#pragma once
#include "AssetImporter.h"
#include "AssetMetadata.h"

namespace Hazel
{
	class AssetManager
	{
	public:
		//using AssetsChangeEventFn = std::function<void(FileSystemChangedEvent)>;

		static void Init();
		//static void SetAssetChangeCallback(const AssetsChangeEventFn& callback);
		static void Shutdown();

		static AssetMetadata& GetMetadata(AssetHandle handle);
		static AssetMetadata& GetMetadata(const std::string& filepath);

		static AssetHandle GetAssetHandleFromFilePath(const std::string& filepath);
		static bool IsAssetHandleValid(AssetHandle assetHandle)
		{
			return GetMetadata(assetHandle).IsValid();
		}

		static void Rename(AssetHandle assetHandle, const std::string& newName);
		static void RemoveAsset(AssetHandle assetHandle);

		static AssetType GetAssetTypeFromFileType(const std::string& extension);

		static AssetHandle ImportAsset(const std::string& filepath);

		template<typename T>
		static Ref<T> GetAsset(AssetHandle assetHandle)
		{
			auto& metadata = GetMetadata(assetHandle);
			HZ_CORE_ASSERT(metadata.IsValid());

			Ref<Asset> asset = nullptr;
			if(!metadata.IsDataLoaded)
			{
				metadata.IsDataLoaded = AssetImporter::TryLoadData(metadata, asset);
				s_LoadedAssets[assetHandle] = asset;
			}
			else
			{
				asset = s_LoadedAssets[assetHandle];
			}
			return asset.As<T>();
		}

		template<typename T>
		static Ref<T> GetAsset(const std::string& filepath)
		{
			return GetAsset<T>(GetAssetHandleFromFilePath(filepath));
		}

	private:
		static void LoadAssetRegistry();
		static void ProcessDirectory(const std::string& directoryPath);
		static void ReloadAssets();
		static void WriteRegistryToFile();

		//static void OnFileSystemChanged(FileSystemChangedEvent e);
		static void OnAssetRenamed(AssetHandle assetHandle, const std::string& newFilePath);

	private:
		static std::unordered_map<AssetHandle, Ref<Asset>> s_LoadedAssets;
		static std::unordered_map<std::string, AssetMetadata> s_AssetRegistry;
		//static AssetsChangeEventFn s_AssetsChangeCallback;
	};
}

#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include "Hazel/Asset/Asset.h"
#include "Hazel/Asset/AssetMetadata.h"
#include "Hazel/Renderer/Texture.h"

#define MAX_INPUT_BUFFER_LENGHT 128
namespace Hazel
{
	struct SelectionStack
	{
	public:
		void Select(AssetHandle item)
		{
			m_Selections.push_back(item);
		}

		void Deselect(AssetHandle item)
		{
			for(auto it = m_Selections.begin(); it != m_Selections.end(); ++it)
			{
				if(*it == item)
				{
					m_Selections.erase(it);
					break;
				}
			}
		}

		bool IsSelected(AssetHandle item) const
		{
			if(m_Selections.size() == 0)
				return false;
			return std::find(m_Selections.begin(), m_Selections.end(), item) != m_Selections.end();
		}

		void Clear()
		{
			if(m_Selections.size() > 0)
				m_Selections.clear();
		}

		size_t SelectionCount() const
		{
			return m_Selections.size();
		}

		AssetHandle* GetSelectionData()
		{
			return m_Selections.data();
		}

		AssetHandle operator[](size_t index) const
		{
			HZ_CORE_ASSERT(index >=0 && index < m_Selections.size());
			return m_Selections[index];
		}
	
	private:
		std::vector<AssetHandle> m_Selections;
	};

	struct DirectoryInfo : public RefCounted
	{
		AssetHandle Handle;
		AssetHandle Parent;

		std::string Name;
		std::string FilePath;

		std::vector<AssetHandle> Assets;
		std::vector<AssetHandle> SubDirectories;
	};

	/**
	 * \brief 这里类似于一个虚拟文件系统
	 */
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		AssetHandle ProcessDirectory(const std::string& directoryPath, AssetHandle parent);
		void DrawDirectoryInfo(AssetHandle directory);

		// render
		void RenderDirectory(Ref<DirectoryInfo>& directory);
		void RenderAsset(AssetMetadata& assetInfo);
		void RenderBottomBar();
		void RenderBreadCrumbs();
		//todo

		void UpdateCurrentDirectory(AssetHandle directoryHandle);
		Ref<DirectoryInfo> GetDirectoryInfo(const std::string& filepath) const;

		// File system event
		void OnDirectoryAdded(const std::string& directoryPath);
	private:
		Ref<Texture2D> m_FileTex;
		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_BackbtnTex;
		Ref<Texture2D> m_FwdbtnTex;
		std::map<std::string, Ref<Texture2D>> m_AssetIconMap;

		bool m_IsDragging = false;
		bool m_IsAnyItemHovered = false;

		char m_RenameBuffer[MAX_INPUT_BUFFER_LENGHT];
		char m_SearchBuffer[MAX_INPUT_BUFFER_LENGHT];

		AssetHandle m_CurrentDirHandle;
		// Assets folder
		AssetHandle m_BaseDirectoryHandle;
		AssetHandle m_PrevDirHandle;
		AssetHandle m_NextDirHandle;
		
		Ref<DirectoryInfo> m_CurrentDirectory;
		Ref<DirectoryInfo> m_BaseDirectory;

		// folder struct
		std::vector<Ref<DirectoryInfo>> m_CurrentDirectories;
		std::vector<AssetMetadata> m_CurrentAssets;
		
		// All directories in Assets folder
		std::unordered_map<AssetHandle, Ref<DirectoryInfo>> m_Directories;
		SelectionStack m_SelectedAssets;
	};
}

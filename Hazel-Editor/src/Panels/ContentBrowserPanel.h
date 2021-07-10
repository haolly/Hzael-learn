#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include "ContentBrowserItem.h"
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

		std::vector<AssetHandle>::iterator begin() { return m_Selections.begin();}	
		std::vector<AssetHandle>::iterator end() { return m_Selections.end();}	

		std::vector<AssetHandle>::const_iterator begin() const { return m_Selections.begin();}	
		std::vector<AssetHandle>::const_iterator end() const { return m_Selections.end();}	
	
	private:
		std::vector<AssetHandle> m_Selections;
	};

	struct ContentBrowserItemList
	{
		static constexpr size_t InvalidItem = std::numeric_limits<size_t>::max();

		std::vector<Ref<ContentBrowserItem>> Items;

		std::vector<Ref<ContentBrowserItem>>::iterator begin() { return Items.begin();}
		std::vector<Ref<ContentBrowserItem>>::iterator end() { return Items.end();}

		std::vector<Ref<ContentBrowserItem>>::const_iterator begin() const { return Items.begin();}
		std::vector<Ref<ContentBrowserItem>>::const_iterator end() const { return Items.end();}

		Ref<ContentBrowserItem>& operator[](size_t index) { return Items[index];}
		const Ref<ContentBrowserItem>& operator[](size_t index) const { return Items[index];}

		size_t FindItem(AssetHandle handle) const
		{
			for(size_t i=0; i< Items.size(); ++i)
			{
				if(Items[i]->GetID() == handle)
					return i;
			}

			return InvalidItem;
		}

		void erase(AssetHandle handle)
		{
			size_t index = FindItem(handle);
			if(index == InvalidItem)
				return;
			auto it = Items.begin() + index;
			Items.erase(it);
		}
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
		AssetHandle ProcessDirectory(const std::string& directoryPath, const Ref<DirectoryInfo> parent);

		void ChangeDirectory(Ref<DirectoryInfo>& directory);

		// render
		void RenderDirectoryHierarchy(Ref<DirectoryInfo>& directory);
		void RenderDirectory(Ref<DirectoryInfo>& directory);
		void RenderAsset(AssetMetadata& assetInfo);
		void RenderBottomBar();
		void RenderBreadCrumbs();
		void RenderTopBar();
		void RenderItems();

		void Refresh();
		void UpdateInput();
		void ClearSelections();

		void RenderDeleteDialogue();
		void RemoveDirectory(Ref<DirectoryInfo>& directory, bool removeFromParent = true);
		
		void SortItemList();

		ContentBrowserItemList Search(const std::string& query, const Ref<DirectoryInfo>& directoryInfo);
		//todo

		Ref<DirectoryInfo> GetDirectoryInfo(const std::string& filepath) const;

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

		ContentBrowserItemList m_CurrentItems;
		
		Ref<DirectoryInfo> m_CurrentDirectory;
		Ref<DirectoryInfo> m_BaseDirectory;
		Ref<DirectoryInfo> m_NextDirectory, m_PreviousDirectory;

		// folder struct
		std::vector<Ref<DirectoryInfo>> m_CurrentDirectories;
		std::vector<AssetMetadata> m_CurrentAssets;
		
		SelectionStack m_SelectionStack;
		// All directories in Assets folder
		std::unordered_map<AssetHandle, Ref<DirectoryInfo>> m_Directories;

	};
}

#pragma once
#include <cstdint>
#include <filesystem>
#include "Hazel/Asset/Asset.h"
#include "Hazel/Core/Ref.h"

namespace Hazel
{
	struct AssetMetadata;
	class Texture2D;

#define MAX_INPUT_BUFFER_LENGTH 128

	enum class ContentBrowserAction
	{
		None = 0,
		Refresh				= BIT(0),
		ClearSelections		= BIT(1),
		Selected			= BIT(2),
		DeSelected			= BIT(3),
		Hovered				= BIT(4),
		Renamed				= BIT(5),
		OpenDeleteDialogue	= BIT(5),
		ShowInExplorer		= BIT(10),
		OpenExternal		= BIT(11),
		Reload				= BIT(12)
	};

	struct CBItemActionResult
	{
		uint16_t Field = 0;

		void Set(ContentBrowserAction flag, bool value)
		{
			if(value)
				Field |= (uint16_t)flag;
			else
				Field &= ~(uint16_t)flag;
		}

		bool IsSet(ContentBrowserAction flag)
		{
			return (uint16_t)flag & Field;
		}
	};

	class ContentBrowserItem : public RefCounted
	{
	public:
		enum class ItemType : uint16_t
		{
			Directory, Asset
		};

	public:
		ContentBrowserItem(ItemType type, AssetHandle id, const std::string& name, const Ref<Texture2D>& icon);
		virtual ~ContentBrowserItem() {}

		void OnRenderBegin();
		CBItemActionResult OnRender(float thumbnailSize);
		void OnRenderEnd();

		virtual void Delete() {}
		virtual bool Move(const std::filesystem::path& destination) { return false;}

		bool IsSelected() const {return m_IsSelected;}

		AssetHandle GetID() const { return m_ID;}
		ItemType GetType() const { return m_Type;}
		const std::string& GetName() const { return m_Name;}

		const Ref<Texture2D>& GetIcon() const { return m_Icon;}

		virtual void Activate(CBItemActionResult& actionResult) {}
		void StartRenaming();
		void SetSelected(bool value);

		void Rename(const std::string& newName, bool fromCallback = false);
	private:
		virtual void OnRenamed(const std::string& newName, bool fromCallback = false)
		{
			m_Name = newName;
		}
		virtual void RenderCustomContextItems() {}
		virtual void UpdateDrop(CBItemActionResult& actionResult){}

		void OnContextMenuOpen(CBItemActionResult& actionResult);

	protected:
		ItemType m_Type;
		AssetHandle m_ID;
		std::string m_Name;
		Ref<Texture2D> m_Icon;

		bool m_IsSelected = false;
		bool m_IsRenaming = false;
		bool m_IsDragging = false;

		friend class ContentBrowserPanel;
	};

	struct DirectoryInfo : public RefCounted
	{
		AssetHandle Handle;
		Ref<DirectoryInfo> Parent;

		std::string Name;

		std::filesystem::path FilePath;

		std::vector<AssetHandle> Assets;
		std::unordered_map<AssetHandle, Ref<DirectoryInfo>> SubDirectories;
	};

	class ContentBrowserDirectory : public ContentBrowserItem
	{
	public:
		ContentBrowserDirectory(const Ref<DirectoryInfo>& directoryInfo, const Ref<Texture2D>& icon);
		virtual ~ContentBrowserDirectory();
	};

	class ContentBrowserAsset : public ContentBrowserItem
	{
	public:
		ContentBrowserAsset(const AssetMetadata& assetInfo, const Ref<Texture2D>& icon);
		virtual ~ContentBrowserAsset();
	};
}

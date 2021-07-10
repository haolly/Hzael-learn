#include "ContentBrowserItem.h"
#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/ImGui/ImGui.h"
#include "imgui/imgui.h"
#include "Hazel/Asset/AssetMetadata.h"

namespace Hazel
{
	static char s_RenameBuffer[MAX_INPUT_BUFFER_LENGTH];

	ContentBrowserItem::ContentBrowserItem(ItemType type, AssetHandle id, const std::string& name,
	                                       const Ref<Texture2D>& icon)
		: m_Type(type), m_ID(id), m_Name(name), m_Icon(icon)
	{
	}

	void ContentBrowserItem::OnRenderBegin()
	{
		ImGui::PushID(&m_ID);
		ImGui::BeginGroup();
	}

	CBItemActionResult ContentBrowserItem::OnRender(float thumbnailSize)
	{
		CBItemActionResult result;
		if (m_IsSelected)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 0.75f));

		UI::ImageButton(m_Name.c_str(), m_Icon, {thumbnailSize, thumbnailSize});

		if (m_IsSelected)
			ImGui::PopStyleColor();

		if (ImGui::BeginPopupContextItem("CBItemContextMenu"))
		{
			result.Set(ContentBrowserAction::Selected, true);
			OnContextMenuOpen(result);
			ImGui::EndPopup();
		}

		if (!m_IsRenaming)
		{
			ImGui::TextWrapped(m_Name.c_str());
			if (Input::IsKeyPressed(KeyCode::HZ_KEY_F2) && m_IsSelected)
				StartRenaming();
		}
		else
		{
			ImGui::SetKeyboardFocusHere();
			if (ImGui::InputText("##rename", s_RenameBuffer, MAX_INPUT_BUFFER_LENGTH,
			                     ImGuiInputTextFlags_EnterReturnsTrue))
			{
				Rename(s_RenameBuffer);
				m_IsRenaming = false;
				result.Set(ContentBrowserAction::Renamed, true);
			}
		}
		return result;
	}

	void ContentBrowserItem::OnRenderEnd()
	{
		ImGui::EndGroup();
		ImGui::PopID();
		ImGui::NextColumn();
	}

	void ContentBrowserItem::StartRenaming()
	{
		if (m_IsRenaming)
			return;
		memset(s_RenameBuffer, 0, MAX_INPUT_BUFFER_LENGTH);
		memcpy(s_RenameBuffer, m_Name.c_str(), m_Name.size());
		m_IsRenaming = true;
	}

	void ContentBrowserItem::SetSelected(bool value)
	{
		m_IsSelected = value;
		if (!m_IsSelected)
		{
			m_IsRenaming = false;
			memset(s_RenameBuffer, 0, MAX_INPUT_BUFFER_LENGTH);
		}
	}

	void ContentBrowserItem::Rename(const std::string& newName, bool fromCallback)
	{
	}

	void ContentBrowserItem::OnContextMenuOpen(CBItemActionResult& actionResult)
	{
		if (ImGui::MenuItem("Reload"))
			actionResult.Set(ContentBrowserAction::Reload, true);

		if (ImGui::MenuItem("Rename"))
			StartRenaming();

		if (ImGui::MenuItem("Delete"))
			actionResult.Set(ContentBrowserAction::OpenDeleteDialogue, true);

		ImGui::Separator();
		if (ImGui::MenuItem("Show In Explorer"))
			actionResult.Set(ContentBrowserAction::ShowInExplorer, true);

		RenderCustomContextItems();
	}

	ContentBrowserDirectory::ContentBrowserDirectory(const Ref<DirectoryInfo>& directoryInfo, const Ref<Texture2D>& icon)
		: ContentBrowserItem(ItemType::Directory, directoryInfo->Handle, directoryInfo->Name, icon)
	{
	}

	ContentBrowserDirectory::~ContentBrowserDirectory()
	{
	}

	ContentBrowserAsset::ContentBrowserAsset(const AssetMetadata& assetInfo, const Ref<Texture2D>& icon)
		: ContentBrowserItem(ItemType::Asset, assetInfo.Handle, assetInfo.FileName, icon)
	{
	}

	ContentBrowserAsset::~ContentBrowserAsset()
	{
	}
}

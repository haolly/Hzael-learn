#include "ContentBrowserPanel.h"

#include <algorithm>
#include <filesystem>
#include <imgui/imgui.h>

#include "Hazel/Asset/AssetManager.h"
#include "Hazel/Utils/PlatformUtils.h"
#include "Hazel/Utils/StringUtils.h"
#include "Hazel/Core/Input.h"
#include "Hazel/ImGui/ImGui.h"
#include "Hazel/Utils/FileSystem.h"
#include "imgui/imgui_internal.h"

namespace Hazel
{
	static float s_ThumbnailSize = 128.0f;
	static float s_Padding = 2.0f;

	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_BaseDirectoryHandle = ProcessDirectory("assets", nullptr);

		m_FileTex = AssetManager::GetAsset<Texture2D>("assets/editor/file.png");
		m_FolderIcon = AssetManager::GetAsset<Texture2D>("assets/editor/folder.png");
		m_AssetIconMap["fbx"] = AssetManager::GetAsset<Texture2D>("assets/editor/fbx.png");
		m_AssetIconMap["obj"] = AssetManager::GetAsset<Texture2D>("assets/editor/obj.png");
		m_AssetIconMap["wav"] = AssetManager::GetAsset<Texture2D>("assets/editor/wav.png");
		m_AssetIconMap["cs"] = AssetManager::GetAsset<Texture2D>("assets/editor/csc.png");
		m_AssetIconMap["png"] = AssetManager::GetAsset<Texture2D>("assets/editor/png.png");
		m_AssetIconMap["hsc"] = AssetManager::GetAsset<Texture2D>("assets/editor/hazel.png");

		m_BackbtnTex = AssetManager::GetAsset<Texture2D>("assets/editor/btn_back.png");
		m_FwdbtnTex = AssetManager::GetAsset<Texture2D>("assets/editor/btn_fwrd.png");

		m_BaseDirectory = m_Directories[m_BaseDirectoryHandle];
		ChangeDirectory(m_BaseDirectory);

		memset(m_RenameBuffer, 0, MAX_INPUT_BUFFER_LENGHT);
		memset(m_SearchBuffer, 0, MAX_INPUT_BUFFER_LENGHT);
	}

	AssetHandle ContentBrowserPanel::ProcessDirectory(const std::string& directoryPath, const Ref<DirectoryInfo> parent)
	{
		Ref<DirectoryInfo> directoryInfo = Ref<DirectoryInfo>::Create();
		directoryInfo->Handle = AssetHandle();
		directoryInfo->Parent = parent;

		directoryInfo->FilePath = directoryPath;
		directoryInfo->Name = Utils::StringUtils::GetFileName(directoryPath);

		for (auto entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_directory())
			{
				AssetHandle subdirHandle = ProcessDirectory(entry.path().string(), directoryInfo);
				directoryInfo->SubDirectories[subdirHandle] = m_Directories[subdirHandle];
				continue;
			}

			auto& metadata = AssetManager::GetMetadata(entry.path().string());
			if (!metadata.IsValid())
			{
				AssetType type = AssetManager::GetAssetTypeFromPath(entry.path());
				if (type == AssetType::None)
					continue;
				metadata.Handle = AssetManager::ImportAsset(entry.path().string());
			}

			if(!metadata.IsValid())
				continue;


			directoryInfo->Assets.push_back(metadata.Handle);
		}

		m_Directories[directoryInfo->Handle] = directoryInfo;
		return directoryInfo->Handle;
	}

	void ContentBrowserPanel::ChangeDirectory(Ref<DirectoryInfo>& directory)
	{
		if (!directory)
			return;

		m_CurrentItems.Items.clear();

		for (auto& [subHandle, subdir] : directory->SubDirectories)
			m_CurrentItems.Items.push_back(Ref<ContentBrowserDirectory>::Create(subdir, m_FolderIcon));

		std::vector<AssetHandle> invalidAssets;
		for (auto assetHandle : directory->Assets)
		{
			AssetMetadata metadata = AssetManager::GetMetadata(assetHandle);

			if (!metadata.IsValid())
				invalidAssets.emplace_back(metadata.Handle);
			else
			{
				auto icon = m_AssetIconMap.find(metadata.Extension) != m_AssetIconMap.end()
					            ? m_AssetIconMap[metadata.Extension]
					            : m_FileTex;
				m_CurrentItems.Items.push_back(Ref<ContentBrowserAsset>::Create(metadata, icon));
			}

			for (auto invalidHandle : invalidAssets)
				directory->Assets.erase(std::remove(directory->Assets.begin(), directory->Assets.end(), invalidHandle),
				                        directory->Assets.end());

			SortItemList();

			m_PreviousDirectory = directory;
			m_NextDirectory = directory;
		}
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
		{
			UI::BeginPropertyGrid();
			{
				ImGui::SetColumnOffset(1, 300);

				ImGui::BeginChild("##folders_common");
				{
					if (ImGui::CollapsingHeader("Content", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
					{
						for (auto& [handle, directory] : m_BaseDirectory->SubDirectories)
						{
							RenderDirectoryHierarchy(directory);
						}
					}
				}
				ImGui::EndChild();

				ImGui::NextColumn();
				ImGui::BeginChild("##directory_structure", ImVec2(0, ImGui::GetWindowHeight() - 65));
				{
					RenderTopBar();
					ImGui::Separator();

					ImGui::BeginChild("Scrolling");
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));

						if (Input::IsKeyPressed(KeyCode::HZ_KEY_ESCAPE) || (ImGui::IsMouseClicked(ImGuiMouseButton_Left)
							&& !m_IsAnyItemHovered))
						{
							m_SelectionStack.Clear();
							memset(m_RenameBuffer, 0, MAX_INPUT_BUFFER_LENGHT);
						}

						m_IsAnyItemHovered = false;

						// Context Menu begin
						if (ImGui::BeginPopupContextWindow(0, ImGuiMouseButton_Right, false))
						{
							if (ImGui::BeginMenu("New"))
							{
								if (ImGui::MenuItem("Folder"))
								{
									FileSystem::CreateFolder(m_CurrentDirectory->FilePath / "New Folder");
								}
								ImGui::EndMenu();
							}

							if (ImGui::MenuItem("Import"))
							{
								std::string filepath = FileDialogs::OpenFile();
								if (!filepath.empty())
								{
									// AssetHandle handle = AssetManager::ImportAsset(filepath);
									// if(handle != 0)
									// {
									// 	m_CurrentDirectory->Assets.push_back(handle);
									// 	UpdateCurrentDirectory(m_CurrentDirHandle);
									// }
								}
							}

							if (ImGui::MenuItem("Refresh"))
							{
								Refresh();
							}

							ImGui::Separator();

							if (ImGui::MenuItem("Show in Explorer"))
								FileSystem::OpenDirectoryInExplorer(m_CurrentDirectory->FilePath);


							ImGui::EndPopup();
						}

						float cellSize = s_ThumbnailSize + s_Padding;
						float panelWidth = ImGui::GetContentRegionAvailWidth();
						int columnCount = (int)(panelWidth / cellSize);
						if(columnCount < 1)
							columnCount = 1;

						ImGui::Columns(columnCount, nullptr, false);

						RenderItems();

						if (ImGui::IsWindowFocused() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
							UpdateInput();

						ImGui::PopStyleColor(2);

						RenderDeleteDialogue();
					}
					ImGui::EndChild();
				}
				ImGui::EndChild();

				RenderBottomBar();
			}
			UI::EndPropertyGrid();
		}
		ImGui::End();
	}

	void ContentBrowserPanel::RenderDirectoryHierarchy(Ref<DirectoryInfo>& directory)
	{
		std::string id = directory->Name + "_TreeNode";
		bool previousState = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetID(id.c_str()));
		bool open = UI::TreeNode(id, directory->Name, ImGuiTreeNodeFlags_SpanAvailWidth);
		if (open)
		{
			for (auto& [handle, child] : directory->SubDirectories)
				RenderDirectoryHierarchy(child);
		}

		if (open != previousState && directory->Handle != m_CurrentDirectory->Handle)
		{
			if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.01f))
			{
				ChangeDirectory(directory);
			}
		}
		if (open)
			ImGui::TreePop();
	}

	void ContentBrowserPanel::RenderDirectory(Ref<DirectoryInfo>& directory)
	{
		ImGui::PushID(&directory->Handle);
		ImGui::BeginGroup();

		bool selected = m_SelectionStack.IsSelected(directory->Handle);

		if (selected)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 0.25f));

		float buttonWidth = ImGui::GetColumnWidth() - 15.0f;
		//UI::ImageButton(directory->FilePath.c_str(), m_FolderIcon->GetImage(), {buttonWidth, buttonWidth});
		//todo

		if (selected)
			ImGui::PopStyleColor();

		//todo

		ImGui::EndGroup();
		ImGui::PopID();
	}

	void ContentBrowserPanel::RenderAsset(AssetMetadata& assetInfo)
	{
	}

	void ContentBrowserPanel::RenderBottomBar()
	{
		ImGui::BeginChild("##panel_controls", ImVec2(ImGui::GetColumnWidth() - 12, 30), false,
		                  ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{
			ImGui::Separator();
			ImGui::Columns(4, 0, false);

			if (m_SelectionStack.SelectionCount() == 1)
			{
				const AssetMetadata& asset = AssetManager::GetMetadata(m_SelectionStack[0]);
				std::string filepath;
				if (asset.IsValid())
				{
					filepath = asset.FilePath;
				}
				else if(m_Directories.find(m_SelectionStack[0]) != m_Directories.end())
				{
					filepath = m_Directories[m_SelectionStack[0]]->FilePath.string();	
					std::replace(filepath.begin(), filepath.end(), '\\', '/');
				}

				ImGui::Text(filepath.c_str());
			}
			else if (m_SelectionStack.SelectionCount() > 1)
			{
				ImGui::Text("%d items selected", m_SelectionStack.SelectionCount());
			}

			ImGui::NextColumn();
			ImGui::NextColumn();
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			ImGui::SliderFloat("##thumbnail_size", &s_ThumbnailSize, 96.0f, 512.0f);
		}
		ImGui::EndChild();
	}

	void ContentBrowserPanel::RenderBreadCrumbs()
	{
	}

	void ContentBrowserPanel::RenderTopBar()
	{
		ImGui::BeginChild("##top_bar", ImVec2(0, 30));
		{
			if (UI::ImageButton(m_BackbtnTex->GetImage(), ImVec2(25, 25)) && m_CurrentDirectory->Handle !=
				m_BaseDirectory->Handle)
			{
				m_NextDirectory = m_CurrentDirectory;
				m_PreviousDirectory = m_CurrentDirectory->Parent;
				ChangeDirectory(m_PreviousDirectory);
			}

			ImGui::SameLine();

			if (UI::ImageButton(m_FwdbtnTex->GetImage(), ImVec2(25, 25)))
			{
				ChangeDirectory(m_NextDirectory);
			}

			ImGui::SameLine();

			{
				ImGui::PushItemWidth(200);
				if (ImGui::InputTextWithHint("", "Search...", m_SearchBuffer, MAX_INPUT_BUFFER_LENGHT))
				{
					if (strlen(m_SearchBuffer) == 0)
					{
						ChangeDirectory(m_CurrentDirectory);
					}
					else
					{
						m_CurrentItems = Search(m_SearchBuffer, m_CurrentDirectory);
						SortItemList();
					}
				}
				ImGui::PopItemWidth();
			}

			ImGui::SameLine();

			//todo
		}
		ImGui::EndChild();
	}

	static std::mutex s_LockMutex;

	void ContentBrowserPanel::RenderItems()
	{
		bool openDeleteDialogue = false;
		std::lock_guard<std::mutex> lock(s_LockMutex);
		for (auto& item : m_CurrentItems)
		{
			item->OnRenderBegin();

			CBItemActionResult result = item->OnRender(s_ThumbnailSize);

			if (result.IsSet(ContentBrowserAction::ClearSelections))
				ClearSelections();

			if (result.IsSet(ContentBrowserAction::Selected) && !m_SelectionStack.IsSelected(item->GetID()))
			{
				m_SelectionStack.Select(item->GetID());
				item->SetSelected(true);
			}

			if (result.IsSet(ContentBrowserAction::DeSelected) && m_SelectionStack.IsSelected(item->GetID()))
			{
				m_SelectionStack.Deselect(item->GetID());
				item->SetSelected(false);
			}

			if (result.IsSet(ContentBrowserAction::Reload))
				AssetManager::ReloadData(item->GetID());

			if(result.IsSet(ContentBrowserAction::OpenDeleteDialogue))
				openDeleteDialogue = true;

			if (result.IsSet(ContentBrowserAction::ShowInExplorer))
			{
				if (item->GetType() == ContentBrowserItem::ItemType::Directory)
					FileSystem::ShowFileInExplorer(m_CurrentDirectory->FilePath / item->GetName());
				else
					FileSystem::ShowFileInExplorer(
						AssetManager::GetFileSystemPath(AssetManager::GetMetadata(item->GetID())));
			}

			item->OnRenderEnd();

			if(result.IsSet(ContentBrowserAction::Renamed))
			{
				SortItemList();
				break;
			}

			if(result.IsSet(ContentBrowserAction::Refresh))
			{
				ChangeDirectory(m_CurrentDirectory);
				break;
			}
		}

		if(openDeleteDialogue)
		{
			ImGui::OpenPopup("Delete");
			openDeleteDialogue = false;
		}
	}

	void ContentBrowserPanel::Refresh()
	{
	}

	void ContentBrowserPanel::UpdateInput()
	{
	}

	void ContentBrowserPanel::ClearSelections()
	{
		for (auto selectedHandle : m_SelectionStack)
		{
			for (auto& item : m_CurrentItems)
			{
				if (item->GetID() == selectedHandle)
					item->SetSelected(false);
			}
		}
		m_SelectionStack.Clear();
	}

	void ContentBrowserPanel::RenderDeleteDialogue()
	{
		// ref https://github.com/ocornut/imgui/issues/1422
		if(ImGui::BeginPopupModal("Delete", 0, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Are you sure you want to delete %d items?", m_SelectionStack.SelectionCount());
			//todo
			ImGui::EndPopup();
		}
	}

	void ContentBrowserPanel::SortItemList()
	{
		std::sort(m_CurrentItems.begin(), m_CurrentItems.end(), [](const Ref<ContentBrowserItem>& item1, const Ref<ContentBrowserItem>& item2)
		{
			if(item1->GetType() == item2->GetType())
				return Utils::StringUtils::ToLower(item1->GetName()) < Utils::StringUtils::ToLower(item2->GetName());

			return (uint16_t)item1->GetType() < (uint16_t)item2->GetType();
		});
	}

	ContentBrowserItemList ContentBrowserPanel::Search(const std::string& query,
		const Ref<DirectoryInfo>& directoryInfo)
	{
		ContentBrowserItemList results;
		std::string queryLowerCase = Utils::StringUtils::ToLower(query);

		for(auto& [handle, subdir] : directoryInfo->SubDirectories)
		{
			std::string subdirName = subdir->FilePath.filename().string();
			if(subdirName.find(queryLowerCase) != std::string::npos)
				results.Items.push_back(Ref<ContentBrowserDirectory>::Create(subdir, m_FolderIcon));

			ContentBrowserItemList list = Search(query, subdir);
			results.Items.insert(results.Items.end(), list.Items.begin(), list.Items.end());
		}

		for(auto& assetHandle : directoryInfo->Assets)
		{
			auto& asset = AssetManager::GetMetadata(assetHandle);
			std::string fileName = Utils::StringUtils::ToLower(asset.FileName);

			if(fileName.find(queryLowerCase) != std::string::npos)
			{
				auto icon = m_AssetIconMap.find(asset.Extension) != m_AssetIconMap.end() ? m_AssetIconMap.at(asset.Extension) : m_FileTex;
				results.Items.push_back(Ref<ContentBrowserAsset>::Create(asset, icon));
			}

			if(queryLowerCase[0] != '.')
				continue;

			if(asset.Extension.find(std::string(&queryLowerCase[1])) != std::string::npos)
			{
				auto icon = m_AssetIconMap.find(asset.Extension) != m_AssetIconMap.end() ? m_AssetIconMap.at(asset.Extension) : m_FileTex;
				results.Items.push_back(Ref<ContentBrowserAsset>::Create(asset, icon));
			}
		}
		return results;
	}

	Ref<DirectoryInfo> ContentBrowserPanel::GetDirectoryInfo(const std::string& filepath) const
	{
		std::string fixedFilepath = filepath;
		std::replace(fixedFilepath.begin(), fixedFilepath.end(), '\\', '/');

		for (auto& [handle, directoryInfo] : m_Directories)
		{
			if (directoryInfo->FilePath == fixedFilepath)
				return directoryInfo;
		}
		return nullptr;
	}
}

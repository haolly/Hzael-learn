#include "ContentBrowserPanel.h"

#include <algorithm>
#include <filesystem>
#include <imgui/imgui.h>

#include "Hazel/Asset/AssetManager.h"
#include "Hazel/Utils/PlatformUtils.h"
#include "Hazel/Utils/StringUtils.h"
#include "../ImGui/ImGui.h"
#include "Hazel/Utils/FileSystem.h"

namespace Hazel
{
	static int s_ColumnCount = 11;
	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_BaseDirectoryHandle = ProcessDirectory("assets", 0);

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
		UpdateCurrentDirectory(m_BaseDirectoryHandle);

		memset(m_RenameBuffer, 0, MAX_INPUT_BUFFER_LENGHT);
		memset(m_SearchBuffer, 0, MAX_INPUT_BUFFER_LENGHT);
	}

	AssetHandle ContentBrowserPanel::ProcessDirectory(const std::string& directoryPath, AssetHandle parent)
	{
		Ref<DirectoryInfo> directoryInfo = Ref<DirectoryInfo>::Create();
		directoryInfo->Handle = AssetHandle();
		directoryInfo->Parent = parent;
		directoryInfo->FilePath = directoryPath;
		std::replace(directoryInfo->FilePath.begin(), directoryInfo->FilePath.end(), '\\', '/');
		directoryInfo->Name = Utils::StringUtils::GetFileName(directoryPath);

		for (auto entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_directory())
			{
				directoryInfo->SubDirectories.push_back(ProcessDirectory(entry.path().string(), directoryInfo->Handle));
				continue;
			}

			const auto& metadata = AssetManager::GetMetadata(entry.path().string());
			if (!metadata.IsValid())
				continue;

			directoryInfo->Assets.push_back(metadata.Handle);
		}

		m_Directories[directoryInfo->Handle] = directoryInfo;
		return directoryInfo->Handle;
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
		{
			UI::BeginPropertyGrid();
			{
				ImGui::SetColumnOffset(1, 240);

				ImGui::BeginChild("##folders_common");
				{
					if (ImGui::CollapsingHeader("Content", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
					{
						for (AssetHandle child : m_BaseDirectory->SubDirectories)
						{
							DrawDirectoryInfo(child);
						}
					}
				}
				ImGui::EndChild();

				ImGui::NextColumn();
				ImGui::BeginChild("##directory_structure", ImVec2(0, ImGui::GetWindowHeight() - 65));
				{
					ImGui::BeginChild("##top_bar", ImVec2(0, 30));
					{
						RenderBreadCrumbs();
					}
					ImGui::EndChild();
					ImGui::Separator();

					ImGui::BeginChild("Scrolling");
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));

						if(Input::IsKeyPressed(KeyCode::HZ_KEY_ESCAPE) || (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !m_IsAnyItemHovered))
						{
							m_SelectedAssets.Clear();
							memset(m_RenameBuffer, 0, MAX_INPUT_BUFFER_LENGHT);
						}

						m_IsAnyItemHovered = false;

						// Context Menu begin
						if(ImGui::BeginPopupContextWindow(0, ImGuiMouseButton_Right, false))
						{
							if(ImGui::BeginMenu("New"))
							{
								if(ImGui::MenuItem("Folder"))
								{
									bool created = FileSystem::CreateFolder(m_CurrentDirectory->FilePath + "/New Folder");
									if(created)
									{
										UpdateCurrentDirectory(m_CurrentDirHandle);
										auto& createdDirectory = GetDirectoryInfo(m_CurrentDirectory->FilePath + "/New Folder");
										//SelectAndStartRenaming()
									}
								}
								ImGui::EndMenu();
							}
							
							if(ImGui::MenuItem("Import"))
							{
								std::string filepath = FileDialogs::OpenFile();
								if(!filepath.empty())
								{
									AssetHandle handle = AssetManager::ImportAsset(filepath);
									if(handle != 0)
									{
										m_CurrentDirectory->Assets.push_back(handle);
										UpdateCurrentDirectory(m_CurrentDirHandle);
									}
								}
							}
							
							if(ImGui::MenuItem("Refresh"))
							{
								UpdateCurrentDirectory(m_CurrentDirHandle);
							}
							ImGui::EndPopup();
						}
						// end

						ImGui::Columns(s_ColumnCount, nullptr, false);
						for(auto& directory: m_CurrentDirectories)
						{
							RenderDirectory(directory);
							ImGui::NextColumn();
						}

						for(auto& asset: m_CurrentAssets)
						{
							RenderAsset(asset);
							ImGui::NextColumn();
						}
						
						ImGui::PopStyleColor(2);
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

	void ContentBrowserPanel::DrawDirectoryInfo(AssetHandle directory)
	{
		const auto& dir = m_Directories[directory];
		if (ImGui::TreeNode(dir->Name.c_str()))
		{
			for (AssetHandle child : dir->SubDirectories)
				DrawDirectoryInfo(child);
			ImGui::TreePop();
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			UpdateCurrentDirectory(directory);
	}

	void ContentBrowserPanel::RenderDirectory(Ref<DirectoryInfo>& directory)
	{
		ImGui::PushID(&directory->Handle);
		ImGui::BeginGroup();

		bool selected = m_SelectedAssets.IsSelected(directory->Handle);

		if(selected)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 0.25f));

		float buttonWidth = ImGui::GetColumnWidth() - 15.0f;
		//UI::ImageButton(directory->FilePath.c_str(), m_FolderIcon->GetImage(), {buttonWidth, buttonWidth});
		//todo

		if(selected)
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

			if(m_SelectedAssets.SelectionCount() == 1)
			{
				const AssetMetadata& asset = AssetManager::GetMetadata(m_SelectedAssets[0]);
				if(asset.IsValid())
					ImGui::Text(asset.FilePath.c_str());
				else
					//directory
					ImGui::Text(m_Directories[m_SelectedAssets[0]]->FilePath.c_str());
			}
			else if(m_SelectedAssets.SelectionCount() > 1)
			{
				ImGui::Text("%d items selected", m_SelectedAssets.SelectionCount());
			}

			ImGui::NextColumn();
			ImGui::NextColumn();
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
			ImGui::SliderInt("##column_count", &s_ColumnCount, 2, 15);
		}
		ImGui::EndChild();
	}

	void ContentBrowserPanel::RenderBreadCrumbs()
	{
	}

	void ContentBrowserPanel::UpdateCurrentDirectory(AssetHandle directoryHandle)
	{
		m_CurrentDirectories.clear();
		m_CurrentAssets.clear();
		m_SelectedAssets.Clear();

		m_CurrentDirHandle = directoryHandle;
		m_CurrentDirectory = m_Directories[directoryHandle];

		for (auto& assetHandle : m_CurrentDirectory->Assets)
			m_CurrentAssets.push_back(AssetManager::GetMetadata(assetHandle));

		for (auto& handle : m_CurrentDirectory->SubDirectories)
			m_CurrentDirectories.push_back(m_Directories[handle]);

		std::sort(m_CurrentDirectories.begin(), m_CurrentDirectories.end(),
		          [](const Ref<DirectoryInfo>& a, const Ref<DirectoryInfo>& b)
		          {
			          return Utils::StringUtils::ToLower(a->Name) < Utils::StringUtils::ToLower(b->Name);
		          });

		std::sort(m_CurrentAssets.begin(), m_CurrentAssets.end(), [](const AssetMetadata& a, const AssetMetadata& b)
		{
			return Utils::StringUtils::ToLower(a.FileName) < Utils::StringUtils::ToLower(b.FileName);
		});
	}

	Ref<DirectoryInfo> ContentBrowserPanel::GetDirectoryInfo(const std::string& filepath) const
	{
		std::string fixedFilepath = filepath;
		std::replace(fixedFilepath.begin(), fixedFilepath.end(), '\\', '/');

		for(auto& [handle, directoryInfo] : m_Directories)
		{
			if(directoryInfo->FilePath == fixedFilepath)
				return directoryInfo;
		}
		return nullptr;
	}

	void ContentBrowserPanel::OnDirectoryAdded(const std::string& directoryPath)
	{
		std::filesystem::path parentPath = directoryPath;
		parentPath = parentPath.parent_path();

		Ref<DirectoryInfo> parentInfo = GetDirectoryInfo(parentPath.string());
		if(parentInfo == nullptr)
		{
			HZ_CORE_ERROR("Add directory outof Asset ?");
			return;
		}

		AssetHandle directoryHandle = ProcessDirectory(directoryPath, parentInfo->Handle);
		Ref<DirectoryInfo> directoryInfo = m_Directories[directoryHandle];
		parentInfo->SubDirectories.push_back(directoryHandle);

		for(auto& entry: std::filesystem::directory_iterator(directoryPath))
		{
			if(entry.is_directory())
			{
				OnDirectoryAdded(entry.path().string());
			}
			else
			{
				AssetHandle handle = AssetManager::ImportAsset(entry.path().string());
				directoryInfo->Assets.push_back(handle);
			}
		}

		std::sort(m_CurrentDirectories.begin(), m_CurrentDirectories.end(),
		          [](const Ref<DirectoryInfo>& a, const Ref<DirectoryInfo>& b)
		          {
			          return Utils::StringUtils::ToLower(a->Name) < Utils::StringUtils::ToLower(b->Name);
		          });
	}
}

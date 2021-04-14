#include <imgui/imgui.h>
#include "SceneHierarchyPanel.h"

#include "glm/gtc/type_ptr.hpp"
#include "Hazel/Scene/Components.h"

namespace Hazel
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.each([&](auto entityHandler)
		{
			Entity entity{entityHandler, m_Context.get()};
			DrawEntityNode(entity);
		});
		if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = {};
		}
		ImGui::End();

		ImGui::Begin("Properties");
		if(m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (m_SelectionContext == entity)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.c_str());

		if(ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}
		
		if(opened)
		{
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if(entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy(buffer, tag.c_str());
			if(ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if(entity.HasComponent<TransformComponent>())
		{
			if(ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,"Transform"))
			{
				auto& transform = entity.GetComponent<TransformComponent>().Transform;

				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.25f);

				ImGui::TreePop();
			}
		}

		if(entity.HasComponent<CameraComponent>())
		{
			if(ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,"Camera"))
			{
				auto& cameraComp = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComp.Camera;

				ImGui::Checkbox("Primary", &cameraComp.Primary);

				const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
				if(ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; ++i)
					{
						bool isSelected = currentProjectionTypeString = projectionTypeStrings[i];
						if(ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if(isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if(camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					
					float fov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if(ImGui::DragFloat("FOV", &fov))
						camera.SetPerspectiveVerticalFOV(glm::radians(fov));

					
					float nearClip = camera.GetPerspectiveNearClip();
					if(ImGui::DragFloat("nearClip", &nearClip))
						camera.SetPerspectiveNearClip(nearClip);

					float farClip = camera.GetPerspectiveFarClip();
					if(ImGui::DragFloat("farClip", &farClip))
						camera.SetPerspectiveFarClip(farClip);
				}
				else if(camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if(ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					
					float nearClip = camera.GetOrthographicNearClip();
					if(ImGui::DragFloat("nearClip", &nearClip))
						camera.SetOrthographicNearClip(nearClip);

					float farClip = camera.GetOrthographicFarClip();
					if(ImGui::DragFloat("farClip", &farClip))
						camera.SetOrthographicFarClip(farClip);

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComp.FixedAspectRatio);
				}

				ImGui::TreePop();
			}
		}
	}
}

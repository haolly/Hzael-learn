#include "hazelPCH.h"
#include "Scene.h"
#include "Hazel/Scene/Components.h"
#include "Hazel/Renderer/Renderer2D.h"
#include "Hazel/Scene/Entity.h"
#include "Hazel/Math/Math.h"
#include "Hazel/Renderer/SceneRenderer.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel
{
	static const std::string DefaultEntityName = "Entity";
	std::unordered_map<UUID, Scene*> s_ActiveScenes;

	struct SceneComponent
	{
		UUID SceneID;
	};

	Scene::Scene(const std::string& debugName, bool isEditorScene)
	{
		m_SceneEntity = m_Registry.create();
		m_Registry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);

		s_ActiveScenes[m_SceneID] = this;
		Init();
	}

	Scene::~Scene()
	{
		m_Registry.clear();
		s_ActiveScenes.erase(m_SceneID);
	}


	void Scene::Init()
	{
	}

	void Scene::OnUpdate(float ts)
	{
		{
			auto view = m_Registry.view<TransformComponent>();
			for(auto entity : view)
			{
				auto& transformComponent = view.get<TransformComponent>(entity);
				Entity e = Entity(entity, this);
				glm::mat4 transform = GetTransformRelativeToParent(e);
				glm::vec3 translation;
				glm::vec3 rotation;
				glm::vec3 scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::quat rotationQuat = glm::quat(rotation);
				transformComponent.Up = glm::normalize(glm::rotate(rotationQuat, glm::vec3(0.0f, 1.0f, 0.0f)));
				transformComponent.Right = glm::normalize(glm::rotate(rotationQuat, glm::vec3(1.0f, 0.0f, 0.0f)));
				transformComponent.Forward = glm::normalize(glm::rotate(rotationQuat, glm::vec3(0.0f, 0.0f, -1.0f)));
			}
		}
	}

	void Scene::OnRenderRuntime(Ref<SceneRenderer> renderer, float ts)
	{
	}

	void Scene::OnRenderEditor(Ref<SceneRenderer> renderer, float ts, const EditorCamera& editorCamera)
	{
		renderer->EndScene();
	}

	void Scene::OnEvent(Event& e)
	{
	}

	void Scene::OnRuntimeStart()
	{
		m_IsPlaying = true;
	}

	void Scene::OnRuntimeStop()
	{
		m_IsPlaying = false;
	}

	void Scene::SetViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	Entity Scene::GetMainCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for(auto entity : view)
		{
			auto& comp = view.get<CameraComponent>(entity);
			if(comp.Primary)
				return {entity, this};
		}
		return {};
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = {m_Registry.create(), this};
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = {};
		entity.AddComponent<TransformComponent>();
		if(!name.empty())
			entity.AddComponent<TagComponent>(name);

		entity.AddComponent<RelationshipComponent>();
		m_EntityIDMap[idComponent.ID] = entity;
		return entity;
	}




	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name, bool runtimeMap)
	{
		auto entity = Entity{m_Registry.create(), this};
		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = uuid;

		entity.AddComponent<TransformComponent>();
		if(!name.empty())
			entity.AddComponent<TagComponent>(name);

		entity.AddComponent<RelationshipComponent>();

		HZ_CORE_ASSERT(m_EntityIDMap.find(uuid) == m_EntityIDMap.end());
		m_EntityIDMap[idComponent.ID] = entity;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity.m_EntityHandle);
	}

	Entity Scene::FindEntityByTag(const std::string& tag)
	{
		auto view = m_Registry.view<TagComponent>();
		for(auto entity : view)
		{
			const auto& canditate = view.get<TagComponent>(entity).Tag;
			if(canditate == tag)
				return Entity(entity, this);
		}

		return Entity{};
	}

	Entity Scene::FindEntityByUUID(UUID id)
	{
		auto view = m_Registry.view<IDComponent>();
		for(auto entity : view)
		{
			const auto& canditate = view.get<IDComponent>(entity);
			if(canditate.ID == id)
				return Entity(entity, this);
		}

		return Entity{};

	}

	void Scene::ConvertToLocalSpace(Entity entity)
	{
	}

	void Scene::ConvertToWorldSpace(Entity entity)
	{
	}

	glm::mat4 Scene::GetTransformRelativeToParent(Entity entity)
	{
		glm::mat4 transform(1.0f);
		Entity parent = FindEntityByUUID(entity.GetParentUUID());
		if(parent)
			transform = GetTransformRelativeToParent(parent);

		return transform * entity.Transform().GetTransform();
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto && entity : view)
		{
			const auto& cameraComp = view.get<CameraComponent>(entity);
			if(cameraComp.Primary)
				return Entity{entity, this};
		}
		return {};
	}

}

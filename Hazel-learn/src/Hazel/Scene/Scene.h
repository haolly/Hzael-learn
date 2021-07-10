#pragma once
#include <entt.hpp>
#include "glm/fwd.hpp"
#include "glm/vec3.hpp"
#include "Hazel/Asset/Asset.h"

namespace Hazel
{
	class Event;
	class SceneRenderer;
	class EditorCamera;
	class Entity;

	struct Light
	{
		glm::vec3 Direction = {0.0f, 0.0f, 0.0f};
		glm::vec3 Radiance = {0.0f, 0.0f, 0.0f};

		float Multipiler = 1.0f;
	};

	struct TransformComponent;

	using EntityMap = std::unordered_map<UUID, Entity>;

	class Scene : public Asset
	{
	public:
		Scene(const std::string& debugName = "Scene", bool isEditorScene = false);
		~Scene();

		void Init();
		void OnUpdate(float ts);
		void OnRenderRuntime(Ref<SceneRenderer> renderer, float ts);
		void OnRenderEditor(Ref<SceneRenderer> renderer, float ts, const EditorCamera& editorCamera);
		void OnEvent(Event& e);

		void OnRuntimeStart();
		void OnRuntimeStop();
		
		void SetViewportResize(uint32_t width, uint32_t height);
		Entity GetMainCameraEntity();
		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityWithID(UUID uuid, const std::string& name = "", bool runtimeMap = false);
		void DestroyEntity(Entity entity);

		template<typename T>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<T>();
		}

		Entity FindEntityByTag(const std::string& tag);
		Entity FindEntityByUUID(UUID id);

		void ConvertToLocalSpace(Entity entity);
		void ConvertToWorldSpace(Entity entity);
		glm::mat4 GetTransformRelativeToParent(Entity entity);

		uint32_t GetViewPortWidth() { return m_ViewportWidth;}
		uint32_t GetViewPortHeight() { return m_ViewportHeight;}
		Entity GetPrimaryCameraEntity();
	private:
		UUID m_SceneID;
		entt::entity m_SceneEntity;
		entt::registry m_Registry;

		std::string m_DebugName;
		bool m_IsEditorScene = false;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		EntityMap m_EntityIDMap;

		entt::entity m_SelectedEntity;
		
		bool m_IsPlaying = false;
		
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;

	};

	

}

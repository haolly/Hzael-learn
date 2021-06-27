#pragma once
#include <entt.hpp>
#include "Hazel/Asset/Asset.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Renderer/SceneRenderer.h"

namespace Hazel
{
	class EditorCamera;
	class Entity;

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
		
		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity entity);
		void OnUpdateEditor(float ts, EditorCamera& camera);
		void OnUpdateRuntime(float ts);
		void OnViewportResize(uint32_t width, uint32_t height);
		uint32_t GetViewPortWidth() { return m_ViewportWidth;}
		uint32_t GetViewPortHeight() { return m_ViewportHeight;}
		Entity GetPrimaryCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		UUID m_SceneID;
		entt::entity m_SceneEntity;
		entt::registry m_Registry;

		std::string m_DebugName;
		bool m_IsEditorScene = false;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		EntityMap m_EntityIDMap;	
		
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}

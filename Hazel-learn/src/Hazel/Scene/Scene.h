#pragma once
#include <entt.hpp>

namespace Hazel
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();
		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity entity);
		void OnUpdate(float ts);
		void OnViewportResize(uint32_t width, uint32_t height);
		uint32_t GetViewPortWidth() { return m_ViewportWidth;}
		uint32_t GetViewPortHeight() { return m_ViewportHeight;}
		Entity GetPrimaryCameraEntity();
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

	

}

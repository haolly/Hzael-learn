#pragma once

#include <entt.hpp>
#include "Hazel/Scene/Scene.h"

namespace Hazel
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);

		template <typename Component, typename... Args>
		decltype(auto) AddComponent(Args&&... args)
		{
			HZ_CORE_ASSERT(!HasComponent<Component>(), "Entity already has the same component");
			return m_Scene->m_Registry.emplace<Component>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template <typename Component>
		decltype(auto) GetComponent()
		{
			HZ_CORE_ASSERT(HasComponent<Component>(), "Entity does not has the component");
			return m_Scene->m_Registry.get<Component>(m_EntityHandle);
		}

		template <typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}

		template <typename T>
		bool RemoveComponent()
		{
			HZ_CORE_ASSERT(HasComponent<Component>(), "Entity does not has the component");
			return m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		bool operator ==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator !=(const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_EntityHandle = entt::null;
		//TODO, SHOULD BE std::weak_ref<Scene>
		Scene* m_Scene = nullptr;
	};
}

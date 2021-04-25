#pragma once

#include <entt.hpp>

#include "Hazel/Scene/Scene.h"

namespace Hazel
{
	//class Scene;

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);

		template <typename Component, typename... Args>
		decltype(auto) AddComponent(Args&&... args)
		{
			HZ_CORE_ASSERT(!HasComponent<Component>(), "Entity already has the same component");
			Component& component = m_Scene->m_Registry.emplace<Component>(m_EntityHandle, std::forward<Args>(args)...);
			//NOTE, 不知道模板特例化为啥报错....
			//原因：include 循环了
			// 当add 一个cameraComp的时候，要去设置camera的viewSize
			m_Scene->OnComponentAdded<Component>(*this, component);
			return component;
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
		void RemoveComponent()
		{
			HZ_CORE_ASSERT(HasComponent<Component>(), "Entity does not has the component");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }

		bool operator ==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator !=(const Entity& other) const { return !(*this == other); }

	public:
		Scene* GetScene() { return m_Scene;}

	private:
		entt::entity m_EntityHandle = entt::null;
		//TODO, SHOULD BE std::weak_ref<Scene>
		Scene* m_Scene = nullptr;
	};
}

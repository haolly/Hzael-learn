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
		void OnUpdate(float ts);
	
	protected:
		entt::registry m_Registry;
		friend class Entity;
	};
}

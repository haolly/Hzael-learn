#pragma once
#include <entt.hpp>

namespace Hazel
{
	class Scene
	{
	public:
		Scene();
		~Scene();
		entt::entity CreateEntity();
		void OnUpdate(float ts);
		//TEMP
		entt::registry m_Registry;
	
	protected:
	};
}

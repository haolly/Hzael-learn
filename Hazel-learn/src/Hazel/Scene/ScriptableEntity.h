#pragma once
#include "Hazel/Scene/Entity.h"
namespace Hazel
{
	class ScriptableEntity
	{
	public:
		template<typename Component>
		decltype(auto) GetComponent()
		{
			return m_Entity.GetComponent<Component>();
		}
	private:
		Entity m_Entity;
		friend class Scene;
	};
}

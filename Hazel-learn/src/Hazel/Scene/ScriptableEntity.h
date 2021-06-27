#pragma once
#include "Hazel/Scene/Entity.h"
namespace Hazel
{
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}
		template<typename Component>
		decltype(auto) GetComponent()
		{
			return m_Entity.GetComponent<Component>();
		}
	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(float ts) {}
	private:
		Entity m_Entity;
		friend class Scene;
	};
}

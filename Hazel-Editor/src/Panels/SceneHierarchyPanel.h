#pragma once
#include "Hazel/Core/Core.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);
		void OnImGuiRender();
		//TODO, ���ﲻӦ����Դͷ������������ѡ����entity����ô������Ҫ�õ���Ӧ��callback
		Entity GetSelectedEntity() const;
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}

#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include <imgui/imgui.h>
#include "EditorLayer.h"

namespace Hazel
{
	class HazelEditor : public Application
	{
	public:
		HazelEditor()
			: Application("Hazel-Editor")
		{
			PushLayer(new EditorLayer());
		}

		virtual ~HazelEditor() = default;
	};

	Application* CreateApplication()
	{
		return new HazelEditor();
	}
}



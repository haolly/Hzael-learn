#include "hazelPCH.h"
#include "Hazel/Core/Application.h"
#include "Hazel/Core/Input.h"

#include <GLFW/glfw3.h>

namespace Hazel
{
	bool Input::IsKeyPressed(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int status = glfwGetKey(window, keycode);
		return status == GLFW_PRESS || status == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int status = glfwGetMouseButton(window, button);
		return status == GLFW_PRESS; 
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().first;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().second;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		auto* const window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return {(float)x, (float)y};
	}
} 

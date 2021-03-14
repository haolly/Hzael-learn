#include "hazelPCH.h"
#include "WindowsInput.h"

#include "Hazel/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Hazel
{
	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int status = glfwGetKey(window, keycode);
		return status == GLFW_PRESS || status == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int status = glfwGetMouseButton(window, button);
		return status == GLFW_PRESS; 
	}

	float WindowsInput::GetMouseXImpl()
	{
		return GetMousePosImpl().first;
	}

	float WindowsInput::GetMouseYImpl()
	{
		return GetMousePosImpl().second;
	}

	std::pair<float, float> WindowsInput::GetMousePosImpl()
	{
		auto* const window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return {(float)x, (float)y};
	}
} 

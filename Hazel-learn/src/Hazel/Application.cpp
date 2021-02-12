#include "hazelPCH.h"
#include "Application.h"
#include "Hazel/Log.h"
#include <GLFW/glfw3.h>

namespace Hazel
{
	Application::Application()
	{
		m_window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Runing)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_window->OnUpdate();
		}
	}
}

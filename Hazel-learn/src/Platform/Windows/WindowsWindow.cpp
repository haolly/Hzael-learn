#include "hazelPCH.h"
#include "WindowsWindow.h"
#include "Hazel/Log.h"

namespace Hazel
{
	static bool s_GLFWInitialized = false;
	inline Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		ShutDown();
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if(enabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
		m_Data.VSync = enabled;
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		HZ_CORE_INFO("Create Window {0} ({1}, {2})", props.Title, props.Width, props.Height);
		if(! s_GLFWInitialized)
		{
			bool success = glfwInit();
			HZ_CORE_ASSERT(success, "GLFW initialize failed");
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}

	void WindowsWindow::ShutDown()
	{
		glfwDestroyWindow(m_Window);
	}
}


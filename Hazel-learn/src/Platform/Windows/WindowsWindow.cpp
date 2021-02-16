#include "hazelPCH.h"
#include "WindowsWindow.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Hazel
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		HZ_CORE_ERROR("GLWF error, {0}, {1}", error, description);
	}

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
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
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
		if (! s_GLFWInitialized)
		{
			bool success = glfwInit();
			HZ_CORE_ASSERT(success, "GLFW initialize failed");
			s_GLFWInitialized = true;
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);

		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();
		
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set event callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			auto data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			WindowResizeEvent e(width, height);
			data.EventCallbackFn(e);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			auto data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent e;
			data.EventCallbackFn(e);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
			case GLFW_PRESS:
				{
					KeyPressedEvent e(key, 0);
					data.EventCallbackFn(e);
					break;
				}
			case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					data.EventCallbackFn(e);
					break;
				}
			case GLFW_REPEAT:
				{
					KeyPressedEvent e(key, 1);
					data.EventCallbackFn(e);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int c)
		{
			auto data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent e(c);
			data.EventCallbackFn(e);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			auto data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
			case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(button);
					data.EventCallbackFn(e);
					break;
				}

			case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent e(button);
					data.EventCallbackFn(e);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			auto data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent e((float)xoffset, (float)yoffset);
			data.EventCallbackFn(e);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			auto data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent e((float)xPos, (float)yPos);
			data.EventCallbackFn(e);
		});
	}

	void WindowsWindow::ShutDown()
	{
		glfwDestroyWindow(m_Window);
	}
}

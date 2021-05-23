#include "hazelPCH.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Hazel/Core/Base.h"

namespace Hazel
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandler)
		:m_WindowHandler(windowHandler)
	{
		HZ_CORE_ASSERT(windowHandler, "Window handler is null!");
	}

	void OpenGLContext::Init()
	{
		HZ_PROFILE_FUNC();
		glfwMakeContextCurrent(m_WindowHandler);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");

		HZ_CORE_INFO("OpenGL Info:");
		HZ_CORE_INFO("	Vendor: {0}", glGetString(GL_VENDOR));
		HZ_CORE_INFO("	Renderer: {0}", glGetString(GL_RENDERER));
		HZ_CORE_INFO("	Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		HZ_PROFILE_FUNC();
		glfwSwapBuffers(m_WindowHandler);
	}
}

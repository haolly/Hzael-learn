#include "hazelPCH.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Hazel/Core.h"
#include "Hazel/Log.h"

namespace Hazel
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandler)
		:m_WindowHandler(windowHandler)
	{
		HZ_CORE_ASSERT(windowHandler, "Window handler is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandler);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandler);
	}
}

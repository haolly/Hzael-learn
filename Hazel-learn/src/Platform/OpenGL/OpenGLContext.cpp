﻿#include "hazelPCH.h"
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

	}

	void OpenGLContext::SwapBuffers()
	{
		HZ_PROFILE_FUNC();
		glfwSwapBuffers(m_WindowHandler);
	}
}

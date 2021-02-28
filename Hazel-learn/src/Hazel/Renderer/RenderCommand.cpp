#include "hazelPCH.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Hazel {
	//TODO, create dynamicly
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}
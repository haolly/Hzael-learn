#include "hazelPCH.h"
#include "Image.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLImage2D.h"

namespace Hazel
{
	Ref<Image2D> Image2D::Create(ImageSpecification specification, Buffer buffer)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPIType::None:
				return nullptr;
			case RendererAPIType::OpenGL:
				return Ref<OpenGLImage2D>::Create(specification, buffer);
		}
		HZ_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}

	Ref<Image2D> Image2D::Create(ImageSpecification specification, const void* data)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPIType::None:
				return nullptr;
			case RendererAPIType::OpenGL:
				return Ref<OpenGLImage2D>::Create(specification, data);
		}
		HZ_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
}

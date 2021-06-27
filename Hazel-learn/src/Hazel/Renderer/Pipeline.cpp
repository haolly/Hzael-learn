#include "hazelPCH.h"
#include "Pipeline.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLPipeline.h"

namespace Hazel
{
	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPIType::None:
			{
				return nullptr;
			}
			case RendererAPIType::OpenGL:
				return Ref<OpenGLPipeline>::Create(spec);
		}

		HZ_CORE_ASSERT(false, "Unknow renderer api");
		return nullptr;
	}
}

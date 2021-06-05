#include "hazelPCH.h"
#include "Material.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLMaterial.h"

namespace Hazel
{
	Ref<Material> Material::Create(const Ref<Shader>& shader, const std::string& name)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPIType::None:
				return nullptr;
			case RendererAPIType::OpenGL:
				return Ref<OpenGLMaterial>::Create(shader, name);
		}
		HZ_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
}

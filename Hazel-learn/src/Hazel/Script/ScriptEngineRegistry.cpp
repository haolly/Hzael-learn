#include "hazelPCH.h"
#include "ScriptEngineRegistry.h"

#include <mono/jit/jit.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/reflection.h>

#include "Hazel/Scene/Components.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel
{
	std::unordered_map<MonoType*, std::function<bool(Entity&)>> s_HasComponentFuncs;
	std::unordered_map<MonoType*, std::function<void(Entity&)>> s_CreateComponentFuncs;
	
	extern MonoImage* s_CoreAssemblyImage;

#define Component_RegisterType(Type) \
	{\
		MonoType* type = mono_reflection_type_from_name("Hazel." #Type, s_CoreAssemblyImage);\
		if(type) {\
			uint32_t id = mono_type_get_type(type);\
			s_HasComponentFuncs[type] = [](Entity& entity){return entity.HasComponent<Type>();}; \
			s_CreateComponentFuncs[type] = [](Entity& entity){entity.AddComponent<Type>(); };\
		} else{\
			HZ_CORE_ERROR("No C# component class found for " #Type);\
		}\
	}

	static void InitComponentTypes()
	{
		Component_RegisterType(TagComponent);
	}
	
	void ScriptEngineRegistry::RegistryAll()
	{
	}
}

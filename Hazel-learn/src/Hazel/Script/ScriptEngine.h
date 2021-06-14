#pragma once

#include <string>

#include "Hazel/Scene/Entity.h"

extern "C" {
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoClass MonoClass;
}

namespace Hazel
{
	enum class FieldType
	{
		None = 0,
		Float,
		Int,
		UnsignedInt,
		String,
		Vec2,
		Vec3,
		Vec4,
		ClassReference
	};

	const char* FieldTypeToString(FieldType type);

	class ScriptEngine
	{
	public:
		static void Init(const std::string& assemblyPath);
		static void ShutDown();

		static MonoObject* Construct(const std::string& fullName, bool callConstructor = true, void** parameters = nullptr);

		static bool LoadHazelRuntimeAssembly(const std::string& path);
		static bool LoadAppAssembly(const std::string& path);

		static bool ModuleExists(const std::string& moduleName);
		
		static void InitScriptEntity(Entity entity);
		static void InstantiateEntityClass(Entity entity);
	};

}


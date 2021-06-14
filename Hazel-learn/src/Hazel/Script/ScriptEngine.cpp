#include "hazelPCH.h"
#include "ScriptEngine.h"
#include "Hazel/Scene/Scene.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <winioctl.h>

#include "Hazel/Scene/Components.h"

namespace Hazel
{
	static MonoDomain* s_CurrentMonoDomain = nullptr;
	//TODO
	static MonoDomain* s_NewMonoDomain = nullptr;
	static std::string s_AssemblyPath;
	static Ref<Scene> s_SceneContext;

	static MonoAssembly* s_AppAssembly = nullptr;
	static MonoAssembly* s_CoreAssembly = nullptr;

	static MonoImage* s_AppAssemblyImage = nullptr;
	static MonoImage* s_CoreAssemblyImage = nullptr;

	// forward declaration
	static MonoMethod* GetMethod(MonoImage* image, const std::string& methodDesc);

	struct EntityScriptClass
	{
		std::string FullName;
		std::string ClassName;
		std::string NamespaceName;

		MonoClass* Class = nullptr;
		MonoMethod* Constructor = nullptr;
		MonoMethod* OnCreateMethod = nullptr;
		MonoMethod* OnDestroyMethod = nullptr;
		MonoMethod* OnUpdateMethod = nullptr;
		MonoMethod* OnPhysicsUpdateMethod = nullptr;

		void InitClassMethods(MonoImage* image)
		{
			Constructor = GetMethod(s_CoreAssemblyImage, "Hazel.Entity:.ctor(ulong)");
			OnCreateMethod = GetMethod(image, FullName + ":OnCreate()");
			OnDestroyMethod = GetMethod(image, FullName + ":OnDestroy()");
			OnUpdateMethod = GetMethod(image, FullName + ":OnUpdate(single)");
			OnPhysicsUpdateMethod = GetMethod(image, FullName + ":OnPhysicsUpdate(single)");
		}
	};
	

	static void InitMono()
	{
		if(!s_CurrentMonoDomain)
		{
			mono_set_assemblies_path("mono/lib");
			auto domain = mono_jit_init("Hazel");

			char* name = "HazelRuntime";
			s_CurrentMonoDomain = mono_domain_create_appdomain(name, nullptr);
		}
	}

	static void ShutdownMono()
	{
		//https://github.com/mono/website/blob/gh-pages/docs/advanced/embedding/index.md#shutting-down-the-runtime
		//mono_jit_cleanup()
	}

	MonoAssembly* LoadAssemblyFromFile(const char* filepath)
	{
		if(filepath==nullptr)
			return nullptr;

		HANDLE file = CreateFileA(filepath, FILE_READ_ACCESS, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(file == INVALID_HANDLE_VALUE)
			return nullptr;

		DWORD fileSize = GetFileSize(file, NULL);
		if(fileSize == INVALID_FILE_SIZE)
		{
			CloseHandle(file);
			return nullptr;
		}

		void* file_data = malloc(fileSize);
		if(file_data == nullptr)
		{
			CloseHandle(file);
			return nullptr;
		}

		DWORD read=0;
		ReadFile(file, file_data, fileSize, &read, nullptr);
		if(fileSize != read)
		{
			free(file_data);
			CloseHandle(file);
			return nullptr;
		}

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(reinterpret_cast<char*>(file_data), fileSize, 1, &status, 0);
		if(status != MONO_IMAGE_OK)
		{
			free(file_data);
			CloseHandle(file);
			mono_image_close(image);
			return nullptr;
		}
		auto assembly = mono_assembly_load_from_full(image, filepath, &status, 0);
		free(file_data);
		CloseHandle(file);
		mono_image_close(image);
		return assembly;
	}

	static MonoAssembly* LoadAssembly(const std::string& path)
	{
		MonoAssembly* assembly = LoadAssemblyFromFile(path.c_str());
		if(!assembly)
		{
			HZ_CORE_ERROR("Could not load assembly: {0}", path);
			return nullptr;
		}
		return assembly;
	}

	static MonoImage* GetAssemblyImage(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		if(!image)
			HZ_CORE_ASSERT(false, "")
		return image;
	}

	static MonoClass* GetClass(MonoImage* image, const EntityScriptClass& scriptClass)
	{
		MonoClass* monoClass = mono_class_from_name(image, scriptClass.NamespaceName.c_str(), scriptClass.ClassName.c_str());
		if(!monoClass)
			HZ_CORE_ERROR("mono_class_from_name failed: {0}: {1}", scriptClass.NamespaceName, scriptClass.ClassName);
		return monoClass;
	}

	// ref https://github.com/mono/website/blob/gh-pages/docs/advanced/embedding/index.md#creating-objects
	static uint32_t Instantiate(EntityScriptClass& scriptClass)
	{
		MonoObject* instance = mono_object_new(s_CurrentMonoDomain, scriptClass.Class);
		if(!instance)
			HZ_CORE_ERROR("mono_object_new failed: {0}", scriptClass.Class);
		mono_runtime_object_init(instance);
		uint32_t handle = mono_gchandle_new(instance, false);
		return handle;
	}

	MonoMethod* GetMethod(MonoImage* image, const std::string& methodDesc)
	{
		MonoMethodDesc* desc = mono_method_desc_new(methodDesc.c_str(), false);
		if(!desc)
			HZ_CORE_ERROR("[ScriptEngine] mono_method_desc_new failed {0}", methodDesc);
		MonoMethod* method = mono_method_desc_search_in_image(desc, image);
		if(!method)
			HZ_CORE_ERROR("[ScriptEngine] mono_method_desc_search_in_image failed {0}", methodDesc);
		return method;
	}

	static MonoObject* CallMethod(MonoObject* object, MonoMethod* method, void** params = nullptr)
	{
		MonoObject* pException = nullptr;
		MonoObject* result = mono_runtime_invoke(method, object, params, &pException);
		return result;
	}

	static void PrintClassMethods(MonoClass* monoClass)
	{
		MonoMethod* iter;
		void* ptr = 0;
		while ((iter = mono_class_get_methods(monoClass, &ptr)) != nullptr)
		{
			printf("--------------------------------------");
			const char* name = mono_method_get_name(iter);
			MonoMethodDesc* methodDesc = mono_method_desc_from_method(iter);

			const char* paramNames = "";
			mono_method_get_param_names(iter, &paramNames);

			printf("Name: %s\n", name);
			printf("Full name:%s\n", mono_method_full_name(iter, true));
			printf("Param Name: %s\n", paramNames);
		}
	}
	
	void ScriptEngine::Init(const std::string& assemblyPath)
	{
		InitMono();
		LoadHazelRuntimeAssembly(assemblyPath);
	}

	void ScriptEngine::ShutDown()
	{
		ShutdownMono();
	}

	MonoObject* ScriptEngine::Construct(const std::string& fullName, bool callConstructor, void** parameters)
	{
		std::string namespaceName;
		std::string className;
		std::string parameterList;

		if(fullName.find(".") != std::string::npos)
		{
			namespaceName = fullName.substr(0, fullName.find_first_of('.'));
			className = fullName.substr(fullName.find_first_of('.') + 1, (fullName.find_first_of(':') - fullName.find_first_of('.')) -1);
		}
		if(fullName.find(":") != std::string::npos)
		{
			parameterList = fullName.substr(fullName.find_first_of(':'));
		}

		MonoClass* clazz = mono_class_from_name(s_CoreAssemblyImage, namespaceName.c_str(), className.c_str());
		MonoObject* obj = mono_object_new(mono_domain_get(), clazz);
		if(callConstructor)
		{
			MonoMethodDesc* desc = mono_method_desc_new(parameterList.c_str(), false);
			MonoMethod* constructor = mono_method_desc_search_in_class(desc, clazz);
			MonoObject* exception = nullptr;
			mono_runtime_invoke(constructor, obj, parameters, &exception);
		}
		return obj;
	}

	static bool s_PostLoadCleanup = false;
	bool ScriptEngine::LoadHazelRuntimeAssembly(const std::string& path)
	{
		s_AssemblyPath = path;
		if(s_CurrentMonoDomain)
		{
			s_NewMonoDomain = mono_domain_create_appdomain("Hazel Runtime", nullptr);
			mono_domain_set(s_NewMonoDomain, false);
			s_PostLoadCleanup = true;
		}

		s_CoreAssembly = LoadAssembly(path);
		if(!s_CoreAssembly)
			return false;

		s_CoreAssemblyImage = GetAssemblyImage(s_CoreAssembly);
		return true;
	}

	//todo, call this when open project
	bool ScriptEngine::LoadAppAssembly(const std::string& path)
	{
		// if(s_AppAssembly)
		// {
		// 	s_AppAssembly = nullptr;
		// 	s_AppAssemblyImage = nullptr;
		// 	return ReloadAssembly(path);
		// }

		auto assembly = LoadAssembly(path);
		if(!assembly)
			return false;

		auto appAssemblyImage = GetAssemblyImage(assembly);
		//ScriptEngineRegistry::RegisterAll();
		//todo
		if(s_PostLoadCleanup)
		{
			mono_domain_unload(s_CurrentMonoDomain);
			s_CurrentMonoDomain = s_NewMonoDomain;
			s_NewMonoDomain = nullptr;
		}

		s_AppAssembly = assembly;
		s_AppAssemblyImage = appAssemblyImage;
		return true;
	}

	bool ScriptEngine::ModuleExists(const std::string& moduleName)
	{
		if(!s_AppAssemblyImage)
			return false;
		std::string NamespaceName, ClassName;
		if(moduleName.find('.') != std::string::npos)
		{
			NamespaceName = moduleName.substr(0, moduleName.find_first_of('.'));
			ClassName = moduleName.substr(moduleName.find_first_of('.') + 1);
		}
		else
		{
			ClassName = moduleName;
		}

		MonoClass* monoClass = mono_class_from_name(s_AppAssemblyImage, NamespaceName.c_str(), ClassName.c_str());
		return monoClass != nullptr;
	}

	//called when an ScriptComponent is been created
	void ScriptEngine::InitScriptEntity(Entity entity)
	{
		Scene* scen = entity.GetScene();
		long long ID = entity.GetComponent<IDComponent>().ID;
		auto& moduleName = entity.GetComponent<ScriptComponent>().ModuleName;
		if(moduleName.empty())
			return;

		if(!ModuleExists(moduleName))
		{
			HZ_CORE_ERROR("Entity reference non-existing script module: {0}", moduleName);
			return;
		}
		EntityScriptClass& scriptClass = s_EntityClassMap[moduleName];
		scriptClass.FullName = moduleName;
		if(moduleName.find('.') != std::string::npos)
		{
			scriptClass.NamespaceName = moduleName.substr(0, moduleName.find_first_of('.'));
			scriptClass.ClassName = moduleName.substr(moduleName.find_first_of('.') + 1);
		}
		else
		{
			scriptClass.ClassName = moduleName;
		}

		scriptClass.Class = GetClass(s_AppAssemblyImage, scriptClass);
		scriptClass.InitClassMethods(s_AppAssemblyImage);
		//todo
	}

	void ScriptEngine::InstantiateEntityClass(Entity entity)
	{
		Scene* scen = entity.GetScene();
		long long id = entity.GetComponent<IDComponent>().ID;
		auto& moduleName = entity.GetComponent<ScriptComponent>().ModuleName;

		EntityInstanceData& entityInstanceData = GetEntityInstanceData()
	}
}

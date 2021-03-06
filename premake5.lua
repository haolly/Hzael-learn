-- premake5.lua
workspace "Hazel-learn"
	architecture "x64"
	configurations { "Debug", "Release", "Dist" }
	startproject "Hazel-Editor"
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Hazel-learn/vendor/GLFW/include"
IncludeDir["Glad"] = "Hazel-learn/vendor/Glad/include"
IncludeDir["imgui"] = "Hazel-learn/vendor/imgui"
IncludeDir["glm"] = "Hazel-learn/vendor/glm"
IncludeDir["stb_image"] = "Hazel-learn/vendor/stb_image"
IncludeDir["entt"] = "Hazel-learn/vendor/entt/include"
IncludeDir["yaml_cpp"] = "Hazel-learn/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "Hazel-learn/vendor/ImGuizmo"

-- include another premake5.lua into this like C language
group "Dependencies"
	include "Hazel-learn/vendor/GLFW"
	include "Hazel-learn/vendor/Glad"
	include "Hazel-learn/vendor/imgui"
	include "Hazel-learn/vendor/yaml-cpp"
group ""

project "Hazel-learn"
	kind "StaticLib"
	location "Hazel-learn"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "hazelPCH.h"
	pchsource "Hazel-learn/src/hazelPCH.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
	}

	-- link static lib
	links
	{
		"GLFW",
		"Glad",
		"imgui",
		"opengl32.lib",
		"yaml-cpp",
	}

	filter "files:Hazel-learn/vendor/ImGuizmo/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"
		defines 
		{
			"HZ_PLATFORM_WINDOWS",
			"HZ_BUILD_DLL",
			"HZ_ENABLE_ASSERT",
			"GLFW_INCLUDE_NONE",
		}

	filter "configurations:Debug"
		defines { "HZ_DEBUG" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "HZ_RELEASE" }
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines { "HZ_DIST" }
		runtime "Release"
		optimize "on"

project "Sandbox"
	kind "ConsoleApp"

	location "Sandbox"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Hazel-learn/vendor/spdlog/include",
		"Hazel-learn/src",
		"Hazel-learn/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
	}

	links {"Hazel-learn"}

	filter "system:windows"
		systemversion "latest"
		defines 
		{
			"HZ_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines { "HZ_DEBUG" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "HZ_RELEASE" }
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines { "HZ_DIST" }
		runtime "Release"
		optimize "on"

project "Hazel-Editor"
	kind "ConsoleApp"

	location "Hazel-Editor"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Hazel-learn/vendor/spdlog/include",
		"Hazel-learn/src",
		"Hazel-learn/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
	}

	links {"Hazel-learn"}

	filter "system:windows"
		systemversion "latest"
		defines 
		{
			"HZ_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines { "HZ_DEBUG" }
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines { "HZ_RELEASE" }
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines { "HZ_DIST" }
		runtime "Release"
		optimize "on"


-- premake5.lua
workspace "Hazel-learn"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
    startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Hazel-learn/vendor/GLFW/include"
IncludeDir["Glad"] = "Hazel-learn/vendor/Glad/include"
IncludeDir["imgui"] = "Hazel-learn/vendor/imgui"

-- include another premake5.lua into this like C language
include "Hazel-learn/vendor/GLFW"
include "Hazel-learn/vendor/Glad"
include "Hazel-learn/vendor/imgui"

project "Hazel-learn"
    kind "SharedLib"
    location "Hazel-learn"
    language "C++"
    staticruntime "Off"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "hazelPCH.h"
    pchsource "Hazel-learn/src/hazelPCH.cpp"
    
    files
    {
         "%{prj.name}/src/**.h",
         "%{prj.name}/src/**.cpp"
    }
    
    includedirs
    {
    	"%{prj.name}/src",
    	"%{prj.name}/vendor/spdlog/include",
    	"%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.imgui}",
    }

    -- link static lib
    links
    {
      "GLFW",
      "Glad",
      "imgui",
      "opengl32.lib",
    }
    
    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"
        defines 
        {
            "HZ_PLATFORM_WINDOWS",
            "HZ_BUILD_DLL",
            "HZ_ENABLE_ASSERT",
            "GLFW_INCLUDE_NONE",
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
        }
         
    
    filter "configurations:Debug"
       defines { "HZ_DEBUG" }
       runtime "Debug"
       symbols "On"
    
    filter "configurations:Release"
       defines { "HZ_RELEASE" }
       runtime "Release"
       optimize "On"

    filter "configurations:Dist"
       defines { "HZ_DIST" }
       runtime "Release"
       optimize "On"

project "Sandbox"
    kind "ConsoleApp"

    location "Sandbox"
    language "C++"
    staticruntime "Off"
    
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
           "Hazel-learn/src"
    }

    links {"Hazel-learn"}
    
    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"
        defines 
        {
            "HZ_PLATFORM_WINDOWS",
        }

    filter "configurations:Debug"
       defines { "HZ_DEBUG" }
       runtime "Debug"
       symbols "On"
    
    filter "configurations:Release"
       defines { "HZ_RELEASE" }
       runtime "Release"
       optimize "On"

    filter "configurations:Dist"
       defines { "HZ_DIST" }
       runtime "Release"
       optimize "On"


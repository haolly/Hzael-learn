-- premake5.lua
workspace "Hazel-learn"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Hazel-learn"
    kind "SharedLib"
    location "Hazel-learn"
    language "C++"
    
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
    	"%{prj.name}/vendor/spdlog/include"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        defines 
        {
            "HZ_PLATFORM_WINDOWS",
            "HZ_BUILD_DLL"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }
         
    
    filter "configurations:Debug"
       defines { "HZ_DEBUG" }
       symbols "On"
    
    filter "configurations:Release"
       defines { "HZ_RELEASE" }
       optimize "On"

    filter "configurations:Dist"
       defines { "HZ_DIST" }
       optimize "On"

project "Sandbox"
    kind "ConsoleApp"

    location "Sandbox"
    language "C++"
    
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
        staticruntime "On"
        systemversion "latest"
        defines 
        {
            "HZ_PLATFORM_WINDOWS",
        }

    filter "configurations:Debug"
       defines { "HZ_DEBUG" }
       symbols "On"
    
    filter "configurations:Release"
       defines { "HZ_RELEASE" }
       optimize "On"

    filter "configurations:Dist"
       defines { "HZ_DIST" }
       optimize "On"


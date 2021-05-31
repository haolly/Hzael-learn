-- premake5.lua
include "Dependencies.lua"

workspace "Hazel-learn"
	architecture "x64"
	configurations { "Debug", "Release", "Dist" }
	startproject "Hazel-Editor"
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include another premake5.lua into this like C language
group "Dependencies"
	include "vendor/premake"
	include "Hazel-learn/vendor/GLFW"
	include "Hazel-learn/vendor/Glad"
	include "Hazel-learn/vendor/imgui"
	include "Hazel-learn/vendor/yaml-cpp"
group ""

include "Hazel-learn"
include "Hazel-Editor"
include "Sandbox"
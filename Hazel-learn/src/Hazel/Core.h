#pragma once

#ifdef HZ_PLATFORM_WINDOWS
	#ifdef HZ_BUILD_DLL
	#define HAZEL_API	__declspec(dllexport)
	#else
	#define HAZEL_API	__declspec(dllimport)
	#endif // HZ_BUILD_DLL
#else
	#error Hazel only run in windows platform
#endif // HZ_PLATFORM_WINDOWS

#ifdef HZ_ENABLE_ASSERT
	#define HZ_ASSERT(x, ...) { if(!x){ HZ_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define HZ_CORE_ASSERT(x, ...) { if(!x){ HZ_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define HZ_ASSERT(x, ...) 
	#define HZ_CORE_ASSERT(x, ...) 
#endif

#define Bit(x) (1 << x)

#define HZ_BIND_EVENT_FN(fn)  std::bind(&fn, this, std::placeholders::_1) 
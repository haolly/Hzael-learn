#pragma once

#include <memory>

#ifdef HZ_PLATFORM_WINDOWS
 #if HZ_DYNAMIC_LINK
	#ifdef HZ_BUILD_DLL
		#define HAZEL_API	__declspec(dllexport)
	#else
		#define HAZEL_API	__declspec(dllimport)
	#endif // HZ_BUILD_DLL
 #else
 	#define HAZEL_API
 #endif
#else
	#error Hazel only run in windows platform
#endif // HZ_PLATFORM_WINDOWS

#ifdef HZ_ENABLE_ASSERT
	#define HZ_ASSERT(x, ...) { if(!x){ HZ_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define HZ_CORE_ASSERT(x, ...) { if(!(x)){ HZ_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define HZ_ASSERT(x, ...) 
	#define HZ_CORE_ASSERT(x, ...) 
#endif

#define Bit(x) (1 << x)

// #define HZ_BIND_EVENT_FN(fn)  std::bind(&fn, this, std::placeholders::_1) 
#define HZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
namespace Hazel {
	/// <summary>
	/// Ϊɶ��Ҫ��Щ�أ�1. ��ͬģ����Ҫǿ���ã����ܳ���Aģ���ͷ���֮����Bģ���ȡ������Դ�� 2. asset ���͵���Դ���ʺϿ���
	/// �����൱��һ���������Դ����
	/// ref https://www.youtube.com/watch?v=HkGZ378nArE&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=41&t=131s
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
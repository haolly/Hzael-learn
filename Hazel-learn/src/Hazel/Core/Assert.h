#pragma once
#include "Hazel/Core/Log.h"

#ifdef HZ_DEBUG
#define HZ_ENABLE_ASSERT
#endif

#define HZ_EXPAND_VARGS(x) x
#ifdef HZ_ENABLE_ASSERT
	#define HZ_ASSERT_NO_MESSAGE(condition) { if(!(condition)){ HZ_ERROR("Assertion failed"); __debugbreak();}}
	#define HZ_ASSERT_MESSAGE(condition, ...) { if(!(condition)){ HZ_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();}}

	#define HZ_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
	// if there is two args, it will be third HZ_ASSERT_MESSAGE, if there is only one args, it will be HZ_ASSERT_NO_MESSAGE
	#define HZ_GET_ASSERT_MACRO(...) HZ_EXPAND_VARGS(HZ_ASSERT_RESOLVE(__VA_ARGS__, HZ_ASSERT_MESSAGE, HZ_ASSERT_NO_MESSAGE))
	//#define HZ_CORE_ASSERT(x, ...) { if(!(x)){ HZ_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define HZ_CORE_ASSERT(...) HZ_EXPAND_VARGS( HZ_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
	#define HZ_ASSERT(x, ...) 
	#define HZ_CORE_ASSERT(x, ...) 
#endif


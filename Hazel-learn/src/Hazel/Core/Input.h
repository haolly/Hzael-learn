#pragma once
#include "Hazel/Core/Base.h"

namespace Hazel
{
	/// <summary>
	/// Input class 在不同的平台只会有一个实例，不会存在runtime时候的切换，所以选择使用哪个inputImpl 实际应该在compile time就决定了；
	/// 相反，renderer API 是应该在不同的平台都包含的，可以在runtime的时候进行切换；
	/// </summary>
	class HAZEL_API Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);

		static std::pair<float, float> GetMousePosition();
		static float GetMouseX(); 
		static float GetMouseY(); 
	};
}

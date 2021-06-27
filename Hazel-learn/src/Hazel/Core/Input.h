#pragma once
#include "KeyCodes.h"
#include "Hazel/Core/Base.h"

namespace Hazel
{
	/// <summary>
	/// </summary>
	class HAZEL_API Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(int button);

		static std::pair<float, float> GetMousePosition();
		static float GetMouseX(); 
		static float GetMouseY(); 
	};
}

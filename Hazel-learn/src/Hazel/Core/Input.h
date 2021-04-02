#pragma once
#include "Hazel/Core/Core.h"

namespace Hazel
{
	/// <summary>
	/// Input class �ڲ�ͬ��ƽֻ̨����һ��ʵ�����������runtimeʱ����л�������ѡ��ʹ���ĸ�inputImpl ʵ��Ӧ����compile time�;����ˣ�
	/// �෴��renderer API ��Ӧ���ڲ�ͬ��ƽ̨�������ģ�������runtime��ʱ������л���
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

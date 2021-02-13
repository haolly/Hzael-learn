#pragma once
#include "Hazel/LayerStack.h"
#include "Hazel/Core.h"
#include "Hazel/Window.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

namespace Hazel
{
	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		std::unique_ptr<Window> m_window;
		bool m_Runing = true;

		LayerStack m_LayerStack;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
	
}


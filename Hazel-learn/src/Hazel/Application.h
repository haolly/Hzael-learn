#pragma once
#include "Hazel/LayerStack.h"
#include "Hazel/Core.h"
#include "Hazel/Window.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/ImGui/ImGuiLayer.h"

namespace Hazel
{
	class Shader;
	class VertexBuffer;
	class IndexBuffer;
	class VertexArray;

	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow()
		{
			return *m_window;
		}

		static Application& Get()
		{
			return *s_Instance;
		}
	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Runing = true;

		LayerStack m_LayerStack;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
	
}


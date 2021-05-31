#pragma once
#include "Hazel/Core/LayerStack.h"
#include "Hazel/Core/Base.h"
#include "Hazel/Core/Window.h"
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
		Application(const std::string& name = "Hazel App");
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow()
		{
			return *m_window;
		}

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application& Get()
		{
			return *s_Instance;
		}
	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Runing = true;
		bool m_Minimized = false;

		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
	
}


#include "hazelPCH.h"
#include "Application.h"

#include "GLFW/glfw3.h"

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;


	Application::Application()
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exist!")
		s_Instance = this;
		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		// 这里不能是 unique_ptr，ownership 的问题，需要更多的去了解背后的原因
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Runing)
		{
			float time = (float)glfwGetTime();		// Platform::GetTime()
			float deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;
			for(Layer* layer: m_LayerStack)
				layer->OnUpdate(deltaTime);

			
			m_ImGuiLayer->Begin();
			for(Layer* layer: m_LayerStack)
				layer->OnImGuiRenderer();
			m_ImGuiLayer->End();
			
			m_window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
		for(auto it=m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if(e.IsHandled())
				break;
		}
			
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}
	
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	bool Application::OnWindowClosed(WindowCloseEvent& e)
	{
		m_Runing = false;
		return true;
	}
}

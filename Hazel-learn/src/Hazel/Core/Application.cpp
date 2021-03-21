#include "hazelPCH.h"
#include "Application.h"

#include "GLFW/glfw3.h"
#include "Hazel/Renderer/Renderer.h"

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;


	Application::Application()
	{
		HZ_PROFILE_FUNC();
		HZ_CORE_ASSERT(!s_Instance, "Application already exist!")
		s_Instance = this;
		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();
		
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		HZ_PROFILE_FUNC();
		while (m_Runing)
		{
			float time = (float)glfwGetTime();		// Platform::GetTime()
			float deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if(!m_Minimized)
			{
				HZ_PROFILE_SCOPE("Update all layer")
				for(Layer* layer: m_LayerStack)
					layer->OnUpdate(deltaTime);
			}

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
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResized));

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

	bool Application::OnWindowResized(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

}

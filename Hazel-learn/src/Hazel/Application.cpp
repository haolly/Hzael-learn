#include "hazelPCH.h"
#include "Application.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Renderer.h"

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

		m_VertexArray.reset(VertexArray::Create());
		// 默认是在clip space 中，[-1, 1] 的范围
		float vertices[3*7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		};


		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)) );
		m_VertexBuffer->Bind();

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}
		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);



		uint32_t indices[3] = {0, 1, 2};

		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(indices[1])));
		m_IndexBuffer->Bind();
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			layout(location=1) in vec4 a_Color;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";
		std::string fragmentSrc = R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}

		)";
		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		// Square
		m_SquareVA.reset(VertexArray::Create());

		float squareVertices[3*4] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f,
		};
		
		m_SquareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)) );
		m_SquareVB->Bind();

		BufferLayout squareLayout = {
			{ShaderDataType::Float3, "a_Position"},
		};
		m_SquareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(m_SquareVB);

		uint32_t squareIndices[3 * 2] = {0, 1, 2, 2, 3, 0};

		m_SquareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(squareIndices[1])));
		m_SquareIB->Bind();
		m_SquareVA->SetIndexBuffer(m_SquareIB);

		std::string blueShaderVertexSrc = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";
		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec3 v_Position;
			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}

		)";

		m_BlueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Runing)
		{
			RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
			RenderCommand::Clear();

			Renderer::BeginScene();

			m_BlueShader->Bind();
			Renderer::Submit(m_SquareVA);

			m_Shader->Bind();
			Renderer::Submit(m_VertexArray);
			
			Renderer::EndScene();

			for(Layer* layer: m_LayerStack)
				layer->OnUpdate();

			
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

#include "Sandbox2D.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

// temporary
#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1920.0f / 1080.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	// Square
	m_SquareVA = Hazel::VertexArray::Create();

	float squareVertices[3*4] = {
		-0.5f, -0.5f, 0.0f, 
		0.5f, -0.5f, 0.0f, 
		0.5f, 0.5f, 0.0f,	 
		-0.5f, 0.5f, 0.0f,	 
	};
	
	Hazel::Ref<Hazel::VertexBuffer> m_SquareVB;
	m_SquareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)) );
	m_SquareVB->Bind();

	Hazel::BufferLayout squareLayout = {
		{Hazel::ShaderDataType::Float3, "a_Position"},
	};
	m_SquareVB->SetLayout(squareLayout);
	m_SquareVA->AddVertexBuffer(m_SquareVB);

	uint32_t squareIndices[3 * 2] = {0, 1, 2, 2, 3, 0};

	Hazel::Ref<Hazel::IndexBuffer> m_SquareIB;
	m_SquareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(squareIndices[1])));
	m_SquareIB->Bind();
	m_SquareVA->SetIndexBuffer(m_SquareIB);

	m_FlatColorShader = shaderLibrary.Load("assets/shaders/FlatColor.glsl");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(float deltaTime)
{
	// Update
	m_CameraController.OnUpdate(deltaTime);

	// Render
	Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
	Hazel::RenderCommand::Clear();

	Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);

	Hazel::Renderer::Submit(m_SquareVA, m_FlatColorShader);

	Hazel::Renderer::EndScene();

}

void Sandbox2D::OnImGuiRenderer()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}

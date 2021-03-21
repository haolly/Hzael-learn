#include "Sandbox2D.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>


Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1920.0f / 1080.0f, true)
{
}

void Sandbox2D::OnAttach()
{
		m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(float deltaTime)
{
	HZ_PROFILE_FUNC();
	// Update
	m_CameraController.OnUpdate(deltaTime);

	// Render
	Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
	Hazel::RenderCommand::Clear();

	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	{
		HZ_PROFILE_SCOPE("DrawQuad");
		Hazel::Renderer2D::DrawQuad({-0.5f, 0.0f}, {0.5f, 1.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
		Hazel::Renderer2D::DrawQuad({0.5f, -0.3f}, {0.5f, 0.5f}, {0.2f, 0.2f, 0.8f, 1.0f});
		Hazel::Renderer2D::DrawQuad({0.8f, -0.3f, -0.1}, {5.0f, 5.0f}, m_Texture);
		Hazel::Renderer2D::DrawQuad({0.8f, -0.3f}, {0.5f, 0.5f}, m_LogoTexture);
		Hazel::Renderer2D::EndScene();
	}

	// std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
	// std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);
	//
	// Hazel::Renderer::Submit(m_SquareVA, m_FlatColorShader);
}

void Sandbox2D::OnImGuiRenderer()
{
	HZ_PROFILE_FUNC();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}

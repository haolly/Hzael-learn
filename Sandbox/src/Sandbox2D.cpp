#include "Sandbox2D.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>


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
		static float rotation = 0.0f;
		rotation += deltaTime * 50.0f;

		HZ_PROFILE_SCOPE("DrawQuad");
		//Full Screen Quad, 这里的缩放大小跟摄像机的left/right,top/bottom 有关, 根据 Texture.glsl 的vertex shader 可知，这里的位置都是世界坐标
		Hazel::Renderer2D::DrawQuad({0.0f, 0.0f}, {1920/1080.0f * 2.0f, 2.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
		//Hazel::Renderer2D::DrawRotatedQuad({0.0f, 0.0f}, 45.0f, {1920/1080.0f * 2.0f, 2.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
		//Hazel::Renderer2D::DrawQuad({0.5f, -0.3f}, {0.5f, 0.5f}, {0.2f, 0.2f, 0.8f, 1.0f});
		//Hazel::Renderer2D::DrawQuad({-5.0f, -5.0f, -0.1}, {10.0f, 10.0f}, m_Texture, 10.0f);
		//Hazel::Renderer2D::DrawQuad({-.0f, -.0f, 0.1}, {1.0f, 1.0f}, m_Texture, 20.0f);
		Hazel::Renderer2D::DrawRotatedQuad({0.0f, 0.0f, 0.1f}, rotation, {0.5f, 0.5f}, m_LogoTexture, 1.0f, glm::vec4(0.8f, 0.2f, 0.1f, 1.0f));
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

#include "Sandbox2D.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "GLFW/include/GLFW/glfw3.h"


Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1920.0f / 1080.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNC();
	m_CheckboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
	m_LogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");
	m_SpriteSheet = Hazel::Texture2D::Create("assets/game/textures/spritesheet.png");
	m_WoodTextureInSheet = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, {463, 1}, {64, 64});

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(float deltaTime)
{
	HZ_PROFILE_FUNC();
	// Update
	m_CameraController.OnUpdate(deltaTime);

	Hazel::Renderer2D::ResetStats();

	// Render
	Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
	Hazel::RenderCommand::Clear();


	{
		static float rotation = 0.0f;
		rotation += deltaTime * 50.0f;

		HZ_PROFILE_SCOPE("DrawQuad");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//Full Screen Quad, 这里的缩放大小跟摄像机的left/right,top/bottom 有关, 根据 Texture.glsl 的vertex shader 可知，这里的位置都是世界坐标
		//Hazel::Renderer2D::DrawQuad({0.0f, 0.0f}, {1920/1080.0f * 2.0f, 2.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
		Hazel::Renderer2D::DrawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
		//Hazel::Renderer2D::DrawRotatedQuad({0.0f, 0.0f}, glm::radians(45.0f), {1920/1080.0f * 2.0f, 2.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
		//Hazel::Renderer2D::DrawQuad({0.5f, -0.3f}, {0.5f, 0.5f}, {0.2f, 0.2f, 0.8f, 1.0f});
		//Hazel::Renderer2D::DrawQuad({-5.0f, -5.0f, -0.1}, {10.0f, 10.0f}, m_CheckboardTexture, 10.0f);
		//Hazel::Renderer2D::DrawQuad({-.0f, -.0f, 0.1}, {1.0f, 1.0f}, m_CheckboardTexture, 20.0f);
		Hazel::Renderer2D::DrawRotatedQuad({0.0f, 0.0f, 0.1f}, glm::radians(rotation), {0.5f, 0.5f}, m_LogoTexture, 1.0f, glm::vec4(0.8f, 0.2f, 0.1f, 1.0f));

		Hazel::Renderer2D::DrawQuad({-0.0f, -0.0f, 0.5}, {1.0f, 1.0f}, m_WoodTextureInSheet, 1.0f);

		Hazel::Renderer2D::EndScene();

		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y+= 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x+= 0.5f)
			{
				glm::vec4 color = { (x + 5) / 10, 0.3, (y + 5) /10,  0.3f};
				Hazel::Renderer2D::DrawQuad({x, y, 0.1f}, {0.45f , 0.45f}, color);
			}
		}
		Hazel::Renderer2D::EndScene();
	}

	if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Hazel::Input::GetMousePos();
		auto width = Hazel::Application::Get().GetWindow().Width();
		auto height = Hazel::Application::Get().GetWindow().Height();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(deltaTime);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());
	HZ_INFO("fps:{0}", 1/deltaTime);
}

void Sandbox2D::OnImGuiRenderer()
{
	HZ_PROFILE_FUNC();
	ImGui::Begin("Settings");

	auto stats = Hazel::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Statistics:");
	ImGui::Text("Draw Count %d:", stats.DrawCalls);
	ImGui::Text("Quad Count %d:", stats.QuadCount);
	ImGui::Text("Vertices %d:", stats.GetTotalVertexCount());
	ImGui::Text("Indices %d:", stats.GetTotatIndexCount());
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));


	ImGui::ColorEdit4("Birth Color", glm::value_ptr(m_Particle.ColorBegin));
	ImGui::ColorEdit4("Death Color", glm::value_ptr(m_Particle.ColorEnd));
	ImGui::DragFloat("Life Time", &m_Particle.LifeTime, 0.1f, 0.0f, 1000.0f);

	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}

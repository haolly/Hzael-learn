#pragma once
#include <Hazel.h>

#include "ParticleSystem.h"

class Sandbox2D : public Hazel::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float deltaTime) override;
	void OnImGuiRenderer() override;
	void OnEvent(Hazel::Event& e) override;
private:
	Hazel::OrthographicCameraController m_CameraController;

	Hazel::ShaderLibrary shaderLibrary;
	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;

	Hazel::Ref<Hazel::Texture2D> m_CheckboardTexture, m_LogoTexture;

	Hazel::Ref<Hazel::Texture2D> m_SpriteSheet;
	Hazel::Ref<Hazel::SubTexture2D> m_WoodTextureInSheet;

	glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.78f, 1.0f};

	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;
};


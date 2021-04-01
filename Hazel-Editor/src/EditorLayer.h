#pragma once
#include <Hazel.h>

namespace Hazel
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnImGuiRenderer() override;
		void OnEvent(Event& e) override;
	private:
		OrthographicCameraController m_CameraController;

		ShaderLibrary shaderLibrary;
		Ref<Shader> m_FlatColorShader;
		Ref<VertexArray> m_SquareVA;

		Ref<Texture2D> m_CheckboardTexture, m_LogoTexture;

		Ref<Texture2D> m_SpriteSheet;
		Ref<SubTexture2D> m_WoodTextureInSheet;
		Ref<Framebuffer> m_Framebuffer;

		glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.78f, 1.0f};
	};
}

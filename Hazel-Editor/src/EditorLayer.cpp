#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "GLFW/include/GLFW/glfw3.h"


namespace Hazel
{
	EditorLayer::EditorLayer()
		: Layer("Sandbox2D"), m_CameraController(1920.0f / 1080.0f, true)
	{
	}

	void EditorLayer::OnAttach()
	{
		HZ_PROFILE_FUNC();
		m_CheckboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Texture2D::Create("assets/textures/ChernoLogo.png");
		m_SpriteSheet = Texture2D::Create("assets/game/textures/spritesheet.png");
		m_WoodTextureInSheet = SubTexture2D::CreateFromCoords(m_SpriteSheet, {463, 1}, {64, 64});

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		HZ_PROFILE_FUNC();
		// Update
		m_CameraController.OnUpdate(deltaTime);

		Renderer2D::ResetStats();


		m_Framebuffer->Bind();
		// Render
		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
		RenderCommand::Clear();


		{
			static float rotation = 0.0f;
			rotation += deltaTime * 50.0f;

			HZ_PROFILE_SCOPE("DrawQuad");
			Renderer2D::BeginScene(m_CameraController.GetCamera());
			//Full Screen Quad, 这里的缩放大小跟摄像机的left/right,top/bottom 有关, 根据 Texture.glsl 的vertex shader 可知，这里的位置都是世界坐标
			//Renderer2D::DrawQuad({0.0f, 0.0f}, {1920/1080.0f * 2.0f, 2.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
			Renderer2D::DrawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
			//Renderer2D::DrawRotatedQuad({0.0f, 0.0f}, glm::radians(45.0f), {1920/1080.0f * 2.0f, 2.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
			//Renderer2D::DrawQuad({0.5f, -0.3f}, {0.5f, 0.5f}, {0.2f, 0.2f, 0.8f, 1.0f});
			//Renderer2D::DrawQuad({-5.0f, -5.0f, -0.1}, {10.0f, 10.0f}, m_CheckboardTexture, 10.0f);
			//Renderer2D::DrawQuad({-.0f, -.0f, 0.1}, {1.0f, 1.0f}, m_CheckboardTexture, 20.0f);
			Renderer2D::DrawRotatedQuad({0.0f, 0.0f, 0.1f}, glm::radians(rotation), {0.5f, 0.5f}, m_LogoTexture,
			                                   1.0f, glm::vec4(0.8f, 0.2f, 0.1f, 1.0f));

			Renderer2D::DrawQuad({-0.0f, -0.0f, 0.5}, {1.0f, 1.0f}, m_WoodTextureInSheet, 1.0f);

			Renderer2D::EndScene();

			Renderer2D::BeginScene(m_CameraController.GetCamera());
			for (float y = -5.0f; y < 5.0f; y += 0.5f)
			{
				for (float x = -5.0f; x < 5.0f; x += 0.5f)
				{
					glm::vec4 color = {(x + 5) / 10, 0.3, (y + 5) / 10, 0.3f};
					Renderer2D::DrawQuad({x, y, 0.1f}, {0.45f, 0.45f}, color);
				}
			}
			Renderer2D::EndScene();
		}

		m_Framebuffer->UnBind();

		HZ_INFO("fps:{0}", 1/deltaTime);
	}

	void EditorLayer::OnImGuiRenderer()
	{
		HZ_PROFILE_FUNC();

		// In 99% case you should be able to just call DockSpaceOverViewport() and ignore all the code below!
		// In this specific demo, we are not using DockSpaceOverViewport() because:
		// - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
		// - we allow the host window to have padding (when opt_padding == true)
		// - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
		// TL;DR; this demo is more complicated than what you would normally use.
		// If we removed all the options we are showcasing, this demo would become:
		//     void ShowExampleAppDockSpace()
		//     {
		//         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		//     }

		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		static bool dockspaceOpen = true;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					Application::Get().Close();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::End();


		ImGui::Begin("Settings");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Statistics:");
		ImGui::Text("Draw Count %d:", stats.DrawCalls);
		ImGui::Text("Quad Count %d:", stats.QuadCount);
		ImGui::Text("Vertices %d:", stats.GetTotalVertexCount());
		ImGui::Text("Indices %d:", stats.GetTotatIndexCount());
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		ImGui::Begin("Viewport");
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		//Note, this panel's size change will not handle by Window:OnEvent
		if(m_ViewportSize != *(glm::vec2*)&viewportPanelSize)
		{
			m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

			m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);
		}
		HZ_INFO("Viewport size: {0}, {1}", viewportPanelSize.x, viewportPanelSize.y);
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0,1}, ImVec2{1, 0});
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}
}

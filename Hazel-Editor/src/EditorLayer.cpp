#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "GLFW/include/GLFW/glfw3.h"


namespace Hazel
{
	EditorLayer::EditorLayer()
		: Layer("Editor"), m_CameraController(1920.0f / 1080.0f, true)
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

		m_ActiveScene = CreateRef<Scene>();
		auto square = m_ActiveScene->CreateEntity();
		m_SquareEntity = square;
		square.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});

		square = m_ActiveScene->CreateEntity();
		square.AddComponent<SpriteRendererComponent>(glm::vec4{1.0f, 1.0f, 0.0f, 1.0f});

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity("Second Camera");
		auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate()
			{
				//GetComponent<TransformComponent>();
				std::cout<< "CameraController: OnCreate" << std::endl;
				m_TransformComp = &GetComponent<TransformComponent>();
			}

			void OnDestroy()
			{
				
			}

			void OnUpdate(float dt)
			{
				auto& translation = m_TransformComp->Translation;
				std::cout<< "CameraController: OnUpdate" << dt<< std::endl;
				float speed = 5.0f;

				if(Input::IsKeyPressed(HZ_KEY_A))
					translation.x -= speed * dt;
				else if(Input::IsKeyPressed(HZ_KEY_D))
					translation.x += speed * dt;

				if(Input::IsKeyPressed(HZ_KEY_W))
					translation.y += speed * dt;
				else if(Input::IsKeyPressed(HZ_KEY_S))
					translation.y -= speed * dt;

				// if (m_EnableRotation)
				// {
				// 	if(Input::IsKeyPressed(HZ_KEY_Q))
				// 		m_CameraRotation += m_CameraRotateSpeed * deltaTime;
				// 	else if(Input::IsKeyPressed(HZ_KEY_E))
				// 		m_CameraRotation -= m_CameraRotateSpeed * deltaTime;
				//
				// 	m_Camera.SetRotation(m_CameraRotation);
				// }
				// m_Camera.SetPosition(m_CameraPosition);
				//
				// // When object is far, move faster, when object is near, move slow
				// m_CameraMoveSpeed = m_ZoomLevel;
			}

		private:
			TransformComponent* m_TransformComp;
		};

		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		HZ_PROFILE_FUNC();

		// Note, 这里渲染相关的要放在update中，不然拖动viewPort的大小改变时候，在 OnImGUIRender 函数中处理就会出现黑框
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (spec.Width != m_ViewportSize.x || spec.Height !=
				m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Update
		if (m_ViewportFocused)
		{
			m_CameraController.OnUpdate(deltaTime);
		}

		Renderer2D::ResetStats();


		m_Framebuffer->Bind();
		// Render
		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
		RenderCommand::Clear();


		//Renderer2D::BeginScene(m_CameraController.GetCamera());
		m_ActiveScene->OnUpdate(deltaTime);
		//Renderer2D::EndScene();

		m_Framebuffer->UnBind();

		HZ_INFO("fps:{0}", 1/deltaTime);
	}

	void EditorLayer::OnImGuiRenderer()
	{
		HZ_PROFILE_FUNC();

		// copy from imgui_demo::ShowExampleAppDockSpace()

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

		m_SceneHierarchyPanel.OnImGuiRender();


		ImGui::Begin("Settings");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Statistics:");
		ImGui::Text("Draw Count %d:", stats.DrawCalls);
		ImGui::Text("Quad Count %d:", stats.QuadCount);
		ImGui::Text("Vertices %d:", stats.GetTotalVertexCount());
		ImGui::Text("Indices %d:", stats.GetTotatIndexCount());

		if (m_SquareEntity)
		{
			ImGui::Separator();
			auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
			ImGui::Text("Tag:%s", tag.c_str());

			auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
			ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));

			auto& squareTrans = m_SquareEntity.GetComponent<TransformComponent>().Translation;
			ImGui::InputFloat3("Square Trans", glm::value_ptr(squareTrans));
			ImGui::Separator();
		}

		ImGui::DragFloat3("Camera Transform", glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Translation));
		if(ImGui::Checkbox("Second Camera:", &m_PrimaryCamera))
		{
			m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
			m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
		}

		{
			auto& camera = m_SecondCamera.GetComponent<CameraComponent>().Camera;
			auto orthSize = camera.GetOrthographicSize();
			if(ImGui::DragFloat("Second Camera Orth Size:", &orthSize))
			{
				camera.SetOrthographicSize(orthSize);
			}
		}
		
		ImGui::End();

		//Viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		ImGui::Begin("Viewport");
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!(m_ViewportFocused || m_ViewportHovered));
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		//Note, this panel's size change will not handle by Window:OnEvent
		m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
		HZ_INFO("Viewport size: {0}, {1}", viewportPanelSize.x, viewportPanelSize.y);
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}
}

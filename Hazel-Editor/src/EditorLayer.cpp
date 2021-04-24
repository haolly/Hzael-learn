#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo/ImGuizmo.h>

#include "Hazel/Scene/SceneSerializer.h"
#include "Hazel/Utils/PlatformUtils.h"
#include "Hazel/Math/Math.h"


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

#if 0
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
#endif

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
		// Limit the minimum width of the property panel
		auto& style = ImGui::GetStyle();
		float minWinSize = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSize;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				// ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				// ImGui::MenuItem("Padding", NULL, &opt_padding);
				// ImGui::Separator();

				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

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


		ImGui::Begin("Stats");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Statistics:");
		ImGui::Text("Draw Count %d:", stats.DrawCalls);
		ImGui::Text("Quad Count %d:", stats.QuadCount);
		ImGui::Text("Vertices %d:", stats.GetTotalVertexCount());
		ImGui::Text("Indices %d:", stats.GetTotatIndexCount());

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
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});

		//Gizmo
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera
			auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			const glm::mat4& cameraProjection = camera.GetProjection();
			glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			//Entity
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			
			//Snapping
			bool snap = Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL);
			float snapValue = 0.5f;
			if(m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;
			float snapValues[3] = {snapValue, snapValue, snapValue};

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			                     (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL,
			                     glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

			
			if(ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);
				tc.Translation = translation;
				// NOTE, to avoid gimbal lock, cause we always ADD values to it
				// 但是，这里也会导致另一个问题， rotation 一直在[-180,180] 之间
				auto deltaRotation = rotation - tc.Rotation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		//Shotcuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL) || Input::IsKeyPressed(HZ_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(HZ_KEY_LEFT_SHIFT) || Input::IsKeyPressed(HZ_KEY_RIGHT_SHIFT);

		switch (e.GetKeyCode())
		{
			case HZ_KEY_S:
			{
				if (control && shift)
				{
					SaveSceneAs();
				}
			}
			break;

			case HZ_KEY_N:
			{
				if (control)
				{
					NewScene();
				}
			}
			break;


			case HZ_KEY_O:
			{
				if (control)
				{
					OpenScene();
				}
			}
			break;

			//Gizmos

			case HZ_KEY_Q:
				m_GizmoType = -1;
				break;
			case HZ_KEY_W:
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case HZ_KEY_E:
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case HZ_KEY_R:
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
		}
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Hazel Scene(*.hazel)\0*.hazel\0");
		if (!filepath.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Hazel Scene(*.hazel)\0*.hazel\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}
}

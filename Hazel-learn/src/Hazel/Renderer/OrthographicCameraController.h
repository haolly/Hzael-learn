#pragma once
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Renderer/OrthographicCamera.h"

namespace Hazel
{
	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRation, bool rotation=false);

		void OnUpdate(float deltaTime);
		void OnEvent(Event& e);
		void Resize(float width, float height);
		OrthographicCamera& GetCamera() { return m_Camera;}
		const OrthographicCamera& GetCamera() const { return m_Camera;}
		const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;
		bool m_EnableRotation;
		glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
		float m_CameraRotation = 0.0f;
		float m_CameraMoveSpeed = 5.0f;
		float m_CameraRotateSpeed = 60.0f;

	};
}


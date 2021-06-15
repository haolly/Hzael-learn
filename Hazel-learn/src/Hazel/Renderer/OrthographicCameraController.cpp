#include "hazelPCH.h"
#include "OrthographicCameraController.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"

namespace Hazel
{
	OrthographicCameraController::OrthographicCameraController(float aspectRation, bool rotation)
		:m_AspectRatio(aspectRation),
		m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }),
		m_Camera(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top),
		m_EnableRotation(rotation)
	{
	}

	void OrthographicCameraController::OnUpdate(float deltaTime)
	{
		if(Input::IsKeyPressed(HZ_KEY_A))
			m_CameraPosition.x -= m_CameraMoveSpeed * deltaTime;
		else if(Input::IsKeyPressed(HZ_KEY_D))
			m_CameraPosition.x += m_CameraMoveSpeed * deltaTime;

		if(Input::IsKeyPressed(HZ_KEY_W))
			m_CameraPosition.y += m_CameraMoveSpeed * deltaTime;
		else if(Input::IsKeyPressed(HZ_KEY_S))
			m_CameraPosition.y -= m_CameraMoveSpeed * deltaTime;

		if (m_EnableRotation)
		{
			if(Input::IsKeyPressed(HZ_KEY_Q))
				m_CameraRotation += m_CameraRotateSpeed * deltaTime;
			else if(Input::IsKeyPressed(HZ_KEY_E))
				m_CameraRotation -= m_CameraRotateSpeed * deltaTime;

			m_Camera.SetRotation(m_CameraRotation);
		}
		m_Camera.SetPosition(m_CameraPosition);

		// When object is far, move faster, when object is near, move slow
		m_CameraMoveSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::Resize(float width, float height)
	{
		m_AspectRatio = width/height;
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		//TODO tweak the zoom speed to respect current zoomLevel like move speed do
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		//TODO, ’‚¿Ô”–Œ Ã‚£¨,,”–±£≥÷œ‘ ,ƒ⁄»›,Û–°≤ª±,°£ ref http://edeleastar.github.io/opengl-programming/topic02/pdf/1.First_Projection.pdf
		// https://paroj.github.io/gltut/Positioning/Tut04%20Aspect%20of%20the%20World.html#idp3329
		Resize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}
}

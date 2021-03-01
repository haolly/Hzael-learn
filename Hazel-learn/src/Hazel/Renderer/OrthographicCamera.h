#pragma once
#include <glm/glm.hpp>

namespace Hazel {
	class OrthographicCamera
	{
	public:
		// -1,1,-1,1 ����clip space �Ĵ�С
		// aspect ratio ��Ӱ����Ⱦ���������ǲ��������Σ�ֻ�и�window��aspect ratio һ���Ż���������
		OrthographicCamera(float left, float right, float bottom, float top);
		
		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; ReculateViewMatrix();}

		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; ReculateViewMatrix();}

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void ReculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position;
		// Only sensible in rotation Z axis, radian
		float m_Rotation = 0.0f;
	};
}

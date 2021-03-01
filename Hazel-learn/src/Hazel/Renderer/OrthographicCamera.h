#pragma once
#include <glm/glm.hpp>

namespace Hazel {
	class OrthographicCamera
	{
	public:
		// -1,1,-1,1 就是clip space 的大小
		// aspect ratio 会影响渲染的正方形是不是正方形；只有跟window的aspect ratio 一样才会是正方形
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

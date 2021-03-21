#pragma once
#include <glm/glm.hpp>

namespace Hazel {
	class OrthographicCamera
	{
	public:
		// aspect ratio 会影响渲染的正方形是不是正方形；只有跟window的aspect ratio 一样才会是正方形
		// left, right, bottom, top 决定能看到的世界坐标的范围，如果摄像机的位置是原点，那么正交摄像机能看到的最左边的地方就是left，最右边的地方就是right
		OrthographicCamera(float left, float right, float bottom, float top);
		void SetProjection(float left, float right, float bottom, float top);
		
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
		// Only sensible in rotation Z axis, degree
		float m_Rotation = 0.0f;
	};
}

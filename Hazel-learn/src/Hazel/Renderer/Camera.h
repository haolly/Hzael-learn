#pragma once

#include <glm/glm.hpp>

namespace Hazel
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			:m_Projection(projection)
		{
		}
		virtual ~Camera() = default;
		const glm::mat4& GetProjection() const
		{
			return m_Projection;
		}
		void SetProjectionMatrix(const glm::mat4& projectionMatrix)
		{
			m_Projection = projectionMatrix;
		}
		float GetExposure() const
		{
			return m_Exposure;
		}
		float& GetExposure()
		{
			return m_Exposure;
		}
	protected:
		//NOTE: camera 自身的fov、far/near 等属性决定了MVP 中的P，V 是由camera的transform决定的
		glm::mat4 m_Projection = glm::mat4(1.0f);
		float m_Exposure = 0.8f;
	};
}


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
	protected:
		//NOTE: camera 自身的fov、far/near 等属性决定了MVP 中的P，V 是由camera的transform决定的
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};
}


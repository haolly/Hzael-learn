#pragma once

#include <glm/glm.hpp>

namespace Hazel
{
	class Camera
	{
	public:
		Camera(const glm::mat4& projection)
			:m_Projection(projection)
		{
		}
		const glm::mat4& GetProjection() const
		{
			return m_Projection;
		}
	private:
		//NOTE: camera 自身的fov、far/near 等属性决定了MVP 中的P，V 是由camera的transform决定的
		glm::mat4 m_Projection;
	};
}


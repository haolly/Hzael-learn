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
		//NOTE: camera �����fov��far/near �����Ծ�����MVP �е�P��V ����camera��transform������
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};
}


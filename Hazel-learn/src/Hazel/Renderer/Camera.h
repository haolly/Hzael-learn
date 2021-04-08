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
		//NOTE: camera �����fov��far/near �����Ծ�����MVP �е�P��V ����camera��transform������
		glm::mat4 m_Projection;
	};
}


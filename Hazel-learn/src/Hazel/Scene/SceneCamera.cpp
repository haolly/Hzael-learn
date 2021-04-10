#include "hazelPCH.h"
#include "SceneCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Hazel
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		//Note, why m_OrthographicSize  does not change ? �� aspectRatio �ı��ʱ�������Ϊԭ����2������ôleft��right �ͻ��󣬴Ӷ���������ĳ���
		m_AspectRatio = (float)width/ (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		float orthoLeft = - m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoRight =  m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoBottom =  -m_OrthographicSize * 0.5f;
		float orthoUp =  m_OrthographicSize * 0.5f;

		m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoUp, m_OrthographicNear, m_OrthographicFar);
	}
}

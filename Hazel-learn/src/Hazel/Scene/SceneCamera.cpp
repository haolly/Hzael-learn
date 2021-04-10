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
		//Note, why m_OrthographicSize  does not change ? 当 aspectRatio 改变的时候，例如变为原来的2倍，那么left，right 就会变大，从而看到更多的场景
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

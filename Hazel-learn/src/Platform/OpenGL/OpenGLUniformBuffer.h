#pragma once
#include "Hazel/Renderer/UniformBuffer.h"

namespace Hazel
{
	class OpenGLUniformBuffer : public Hazel::UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer();


		void SetData(const void* data, uint32_t size, uint32_t offset) override;
	private:
		uint32_t m_RendererID = 0;
	};
}

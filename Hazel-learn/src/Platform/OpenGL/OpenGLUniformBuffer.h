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
		void RT_SetData(const void* data, uint32_t size, uint32_t offset) override;
		uint32_t GetBinding() const override {return m_Binding;}
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Size = 0;
		uint32_t m_Binding = 0;
		uint8_t* m_LocalStorage = nullptr;
	};
}

#pragma once
#include "Hazel/Renderer/UniformBufferSet.h"

namespace Hazel
{
	//TODO, 这个类没有完全实现
	class OpenGLUniformBufferSet : public UniformBufferSet
	{
	public:
		OpenGLUniformBufferSet(uint32_t frames) {}
		~OpenGLUniformBufferSet() override = default;
		//TODO, need implementation
		void Create(uint32_t size, uint32_t binding) override {}
		//TODO, need implementation
		Ref<UniformBuffer> Get(uint32_t binding, uint32_t set, uint32_t frame) override { return nullptr;}
		void Set(Ref<UniformBuffer> uniformBuffer, uint32_t set, uint32_t frame) override {}
	};
}

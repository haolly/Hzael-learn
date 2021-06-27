#pragma once
#include <map>

#include "Hazel/Renderer/UniformBufferSet.h"

namespace Hazel
{
	//TODO liuhao , what is the purpose of frame and set?
	class OpenGLUniformBufferSet : public UniformBufferSet
	{
	public:
		OpenGLUniformBufferSet(uint32_t frames) : m_Frames(frames)
		{}
		~OpenGLUniformBufferSet() override = default;
		void Create(uint32_t size, uint32_t binding) override
		{
			for(uint32_t frame=0; frame < m_Frames; frame++)
			{
				const Ref<UniformBuffer> uniformBuffer = UniformBuffer::Create(size, binding);
				Set(uniformBuffer, 0, frame);
			}
		}
		Ref<UniformBuffer> Get(uint32_t binding, uint32_t set, uint32_t frame) override
		{
			HZ_CORE_ASSERT(m_UniformBuffers.find(frame) != m_UniformBuffers.end());
			HZ_CORE_ASSERT(m_UniformBuffers.at(frame).find(set) != m_UniformBuffers.at(frame).end());
			HZ_CORE_ASSERT(m_UniformBuffers.at(frame).at(set).find(binding) != m_UniformBuffers.at(frame).at(set).end());
			return m_UniformBuffers.at(frame).at(set).at(binding);
		}
		void Set(Ref<UniformBuffer> uniformBuffer, uint32_t set, uint32_t frame) override
		{
			m_UniformBuffers[frame][set][uniformBuffer->GetBinding()] = uniformBuffer;
		}

	private:
		uint32_t m_Frames;
		std::map<uint32_t, std::map<uint32_t, std::map<uint32_t, Ref<UniformBuffer>>>> m_UniformBuffers;	// frame->set->binding
	};
}

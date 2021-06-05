#include "hazelPCH.h"
#include "RenderCommandQueue.h"

namespace Hazel
{
	RenderCommandQueue::RenderCommandQueue()
	{
		// continues buffer
		m_CommandBuffer = new uint8_t[10 * 1024 * 1024];	//10mb
		// head pointer, will increase when allocate
		m_CommandBufferPtr = m_CommandBuffer;
		memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
	}

	RenderCommandQueue::~RenderCommandQueue()
	{
		delete[] m_CommandBuffer;
	}

	void* RenderCommandQueue::Allocate(RenderCommandFn func, uint32_t size)
	{
		*(RenderCommandFn*)m_CommandBufferPtr = func;
		m_CommandBufferPtr += sizeof(RenderCommandFn);

		*(uint32_t*)m_CommandBufferPtr = size;
		m_CommandBufferPtr += sizeof(uint32_t);

		void* memory = m_CommandBufferPtr;
		m_CommandBufferPtr += size;

		m_CommandCount++;
		return memory;
	}

	void RenderCommandQueue::Execute()
	{
		byte* buffer = m_CommandBuffer;

		for(uint32_t i=0; i< m_CommandCount; ++i)
		{
			RenderCommandFn function = *(RenderCommandFn*)buffer;
			buffer += sizeof(RenderCommandFn);

			// parameter size
			uint32_t size = *(uint32_t*)buffer;
			buffer += sizeof(uint32_t);
			// call function with parameter blob, the parameter blob is actually a lambda object created by placement new
			function(buffer);
			
			buffer += size;
		}

		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}
}

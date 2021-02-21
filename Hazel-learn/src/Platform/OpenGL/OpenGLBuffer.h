#pragma once
#include "hazelPCH.h"
#include "Hazel/Renderer/Buffer.h"

namespace Hazel {
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();
		void Bind() const override;

		void UnBind() const override;

		virtual void SetLayout(const BufferLayout& layout) override 
		{
			m_Layout = layout;
		}

		virtual const BufferLayout& GetLayout() const override { return m_Layout;}

	private:
		uint32_t  m_RendererID;
		BufferLayout m_Layout;

	};


	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~OpenGLIndexBuffer();
		void Bind() const override;

		void UnBind() const override;

		virtual uint32_t GetCount() const override;

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;

	};
}

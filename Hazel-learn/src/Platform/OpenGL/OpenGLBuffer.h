#pragma once
#include "hazelPCH.h"
#include "Hazel/Renderer/Buffer.h"

namespace Hazel {
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		explicit OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();
		void Bind() const override;
		void UnBind() const override;
		void SetData(const void* data, uint32_t dataSize) override;

		virtual void SetLayout(const VertexBufferLayout& layout) override 
		{
			m_Layout = layout;
		}

		virtual const VertexBufferLayout& GetLayout() const override { return m_Layout;}

	private:
		uint32_t  m_RendererID;
		VertexBufferLayout m_Layout;

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

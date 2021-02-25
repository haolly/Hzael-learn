#include "hazelPCH.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Hazel {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Hazel::ShaderDataType::Float:
			return GL_FLOAT;
		case Hazel::ShaderDataType::Float2:
			return GL_FLOAT;
			break;
		case Hazel::ShaderDataType::Float3:
			return GL_FLOAT;
			break;
		case Hazel::ShaderDataType::Float4:
			return GL_FLOAT;
			break;
		case Hazel::ShaderDataType::Mat3:
			return GL_FLOAT;
			break;
		case Hazel::ShaderDataType::Mat4:
			return GL_FLOAT;
			break;
		case Hazel::ShaderDataType::Int:
			return GL_INT;
			break;
		case Hazel::ShaderDataType::Int2:
			return GL_INT;
			break;
		case Hazel::ShaderDataType::Int3:
			return GL_INT;
			break;
		case Hazel::ShaderDataType::Int4:
			return GL_INT;
			break;
		case Hazel::ShaderDataType::Bool:
			return GL_BOOL;
			break;
		}
		HZ_CORE_ASSERT(false, "Unknow ShaderDataType");
	}


	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		HZ_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has not layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		uint32_t index = 0;
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, 
								element.GetComponentCount(), 
								ShaderDataTypeToOpenGLBaseType(element.Type), 
								element.Normalized ? GL_TRUE : GL_FALSE, 
								layout.GetStride(), 
								(const void*)element.Offset);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

	const std::vector<std::shared_ptr<VertexBuffer>>& OpenGLVertexArray::GetVertexBuffers() const
	{
		return m_VertexBuffers;
	}

	const std::shared_ptr<IndexBuffer>& OpenGLVertexArray::GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}

}

#include "hazelPCH.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Hazel
{
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

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		HZ_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has not layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(m_VertexBuferIndex);
					glVertexAttribPointer(m_VertexBuferIndex,
										  element.GetComponentCount(),
										  ShaderDataTypeToOpenGLBaseType(element.Type),
										  element.Normalized ? GL_TRUE : GL_FALSE,
										  layout.GetStride(),
										  (const void*)element.Offset);
					break;
				}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(m_VertexBuferIndex);
					glVertexAttribIPointer(m_VertexBuferIndex,
										  element.GetComponentCount(),
										  ShaderDataTypeToOpenGLBaseType(element.Type),
										  layout.GetStride(),
										  (const void*)element.Offset);
					break;
				}
			}
			m_VertexBuferIndex++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

	const std::vector<Ref<VertexBuffer>>& OpenGLVertexArray::GetVertexBuffers() const
	{
		return m_VertexBuffers;
	}

	const Ref<IndexBuffer>& OpenGLVertexArray::GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}
}

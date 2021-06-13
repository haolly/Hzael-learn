#include "hazelPCH.h"
#include "OpenGLPipeline.h"

#include "glad/glad.h"
#include "Hazel/Renderer/Renderer.h"

namespace Hazel
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float: return GL_FLOAT;
			case ShaderDataType::Float2: return GL_FLOAT;
			case ShaderDataType::Float3: return GL_FLOAT;
			case ShaderDataType::Float4: return GL_FLOAT;
			case ShaderDataType::Mat3: return GL_FLOAT;
			case ShaderDataType::Mat4: return GL_FLOAT;
			case ShaderDataType::Int: return GL_INT;
			case ShaderDataType::Int2: return GL_INT;
			case ShaderDataType::Int3: return GL_INT;
			case ShaderDataType::Int4: return GL_INT;
			case ShaderDataType::Bool: return GL_BOOL;
		}
		HZ_CORE_ASSERT(false, "Unknow ShaderDataType");
		return 0;
	}
	
	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
		: m_Specification(spec)
	{
		HZ_CORE_ASSERT(spec.Shader, "");
		HZ_CORE_ASSERT(spec.RenderPass, "");
		Invalidate();
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		GLuint rendererID = m_VertexArrayRendererID;
		Renderer::Submit([rendererID]()
		{
			glDeleteVertexArrays(1, &rendererID);
		});
	}

	void OpenGLPipeline::Invalidate()
	{
		HZ_CORE_ASSERT(m_Specification.Layout.GetElements().size(), "Layout is empty");
		Ref<OpenGLPipeline> instance = this;
		Renderer::Submit([instance]() mutable
		{
			auto& vertexArrayRendererID = instance->m_VertexArrayRendererID;
			if(vertexArrayRendererID)
				glDeleteVertexArrays(1, &vertexArrayRendererID);

			glGenVertexArrays(1, &vertexArrayRendererID);
			glBindVertexArray(vertexArrayRendererID);
			glBindVertexArray(0);
		});
	}

	void OpenGLPipeline::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set)
	{
	}

	void OpenGLPipeline::Bind()
	{
		Ref<OpenGLPipeline> instance = this;
		Renderer::Submit([instance]()
		{
			glBindVertexArray(instance->m_VertexArrayRendererID);
			const auto& layout = instance->m_Specification.Layout;
			uint32_t attributeIndex = 0;
			for(const auto& element : layout)
			{
				const auto glBaseType = ShaderDataTypeToOpenGLBaseType(element.Type);
				glEnableVertexAttribArray(attributeIndex);
				if(glBaseType == GL_INT)
				{
					glVertexAttribIPointer(attributeIndex, element.GetComponentCount(), glBaseType, layout.GetStride(), (const void*)element.Offset);	
				}
				else
				{
					glVertexAttribPointer(attributeIndex, element.GetComponentCount(), glBaseType, element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(),
						(const void*)element.Offset);
				}
				attributeIndex++;
			}
		});
	}
}

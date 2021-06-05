#pragma once
#include "Hazel/Renderer/Pipeline.h"

namespace Hazel
{
	class OpenGLPipeline : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		~OpenGLPipeline();

		PipelineSpecification& GetSpecification() override { return m_Specification;}
		const PipelineSpecification& GetSpecification() const override { return m_Specification; }
		void Invalidate() override;
		void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set) override;
		void Bind() override;
	private:
		PipelineSpecification m_Specification;
		uint32_t m_VertexArrayRendererID = 0;
	};
}

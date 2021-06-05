#pragma once
#include "Hazel/Renderer/RenderPass.h"

namespace Hazel
{
	//TODO, 啥都不干？
	class OpenGLRenderPass : public RenderPass
	{
	public:
		OpenGLRenderPass(const RenderPassSpecification& spec);
		~OpenGLRenderPass() override;
		RenderPassSpecification& GetSpecification() override { return m_Specification; }
		const RenderPassSpecification& GetSpecification() const override { return m_Specification;}
	private:
		RenderPassSpecification m_Specification;
	};
}

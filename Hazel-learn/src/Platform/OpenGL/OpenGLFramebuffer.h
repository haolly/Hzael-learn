#pragma once

#include "Hazel/Renderer/Framebuffer.h"

namespace Hazel
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();
		const FramebufferSpecification& GetSpecification() const override
		{
			return m_Specification;
		}
		void Invalidate();
		void Bind() const override;
		void UnBind() const override;
		uint32_t GetColorAttachmentRendererID() const override
		{
			return m_ColorAttachment;
		}
	private:
		FramebufferSpecification m_Specification;
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment;
		uint32_t m_DepthAttachment;
	};
}


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
		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override
		{
			HZ_CORE_ASSERT(index < m_ColorAttachments.size(), "");
			return m_ColorAttachments[index];
		}

		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		void ClearAttachment(uint32_t attachmentIndex, int data) override;
	private:
		FramebufferSpecification m_Specification;
		uint32_t m_RendererID = 0;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
		FramebufferTextureSpecification m_DepthAttachmentSpec = FramebufferTextureSpecification(FramebufferTextureFormat::None);

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment;
	};
}


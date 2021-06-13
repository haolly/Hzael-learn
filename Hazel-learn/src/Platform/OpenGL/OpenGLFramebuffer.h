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
		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const
		{
			HZ_CORE_ASSERT(index < m_ColorAttachments.size(), "");
			return m_ColorAttachments[index];
		}

		// int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		// void ClearAttachment(uint32_t attachmentIndex, int data) override;
		
		void Resize(uint32_t width, uint32_t height, bool forceRecreate) override;
		void AddResizeCallback(const std::function<void(Ref<Framebuffer>)>& func) override;
		void BindTexture(uint32_t attachmentIndex, uint32_t slot) const override;
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }
		uint32_t GetRendererID() const override { return m_RendererID; }
		Ref<Image2D> GetImage(uint32_t attachmentIndex) const override
		{
			HZ_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "");
			return m_ColorAttachments.at(attachmentIndex);
		}
		Ref<Image2D> GetDepthImage() const override { return m_DepthAttachment; }
	private:
		FramebufferSpecification m_Specification;
		uint32_t m_RendererID = 0;

		std::vector<Ref<Image2D>> m_ColorAttachments;
		Ref<Image2D> m_DepthAttachment;
		
		std::vector<ImageFormat> m_ColorAttachmentFormats;
		ImageFormat m_DepthAttachmentFormat = ImageFormat::None;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}


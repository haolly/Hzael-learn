#pragma once

namespace Hazel
{
	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;
		//FramebufferFormat Format = 
		// Render pass ���õ�������һ��render pass ��ʵ����һ�� framebuffer
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		//TODO, REMOVE
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;
		virtual uint32_t GetColorAttachmentRendererID() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}


#pragma once

namespace Hazel
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8,
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;

		explicit FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		//todo: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}
		std::vector<FramebufferTextureSpecification> Attachments;
	};
	
	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;
		FramebufferAttachmentSpecification Attachments;
		// Render pass 会用到这个概念，一个render pass 其实就是一个 framebuffer
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		//TODO, REMOVE
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		
		virtual ~Framebuffer() = default;
		
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int data) = 0;
		
		virtual const FramebufferSpecification& GetSpecification() const = 0;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}


#pragma once
#include <map>

#include "Image.h"
#include "glm/vec4.hpp"

namespace Hazel
{
	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;

		explicit FramebufferTextureSpecification(ImageFormat format)
			: TextureFormat(format) {}

		ImageFormat TextureFormat = ImageFormat::None;
		//todo: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification>& attachments)
			: Attachments(attachments) {}
		std::vector<FramebufferTextureSpecification> Attachments;
	};
	
	struct FramebufferSpecification
	{
		float Scale = 1.0f;
		uint32_t Width, Height;
		glm::vec4 ClearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		//todo
		bool ClearOnLoad = true;

		
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;
		
		// Render pass 会用到这个概念，一个render pass 其实就是一个 framebuffer
		bool SwapChainTarget = false;

		std::map<uint32_t, Ref<Image2D>> ExistingImages;

		std::string DebugName;
	};

	class Framebuffer : public RefCounted
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		
		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) = 0;
		virtual void AddResizeCallback(const std::function<void(Ref<Framebuffer>)>& func) = 0;

		virtual void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot=0) const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		virtual Ref<Image2D> GetImage(uint32_t attachmentIndex = 0) const = 0;
		virtual Ref<Image2D> GetDepthImage() const = 0;
		
		virtual const FramebufferSpecification& GetSpecification() const = 0;
		
		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};

	class FramebufferPool final
	{
	public:
		FramebufferPool(uint32_t maxFramebuffers = 32);
		~FramebufferPool();

		std::weak_ptr<Framebuffer> AllocateBuffer();
		void Add(const Ref<Framebuffer>& framebuffer);

		std::vector<Ref<Framebuffer>>& GetAll() { return m_Pool;}
		const std::vector<Ref<Framebuffer>>& GetAll() const { return m_Pool; }

		inline static FramebufferPool* GetGlobal() { return s_Instance;}
	private:
		std::vector<Ref<Framebuffer>> m_Pool;
		static FramebufferPool* s_Instance;
	};
}


#include "hazelPCH.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

#include "OpenGLImage2D.h"
#include "Hazel/Core/Application.h"
#include "Hazel/Renderer/Renderer.h"

namespace Hazel
{
	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static GLenum DepthAttachmentType(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::DEPTH32F: return GL_DEPTH_ATTACHMENT;
				case ImageFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL_ATTACHMENT;
			}
			HZ_CORE_ASSERT(false, "unknow format");
			return 0;
		}

		static Ref<Image2D> CreateAndAttachColorAttachment(int samples, ImageFormat format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			Ref<Image2D> image;
			if(multisampled)
			{
				
			}
			else
			{
				ImageSpecification spec;
				spec.Format = format;
				spec.Width = width;
				spec.Height = height;
				image = Image2D::Create(spec);
				image->Invalidate();
			}
			Ref<OpenGLImage2D> glImage = image.As<OpenGLImage2D>();
			glImage->CreateSampler(TextureProperties());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), glImage->GetRendererID(), 0);
			return image;
		}

		static Ref<Image2D> AttachDepthTexture(int samples, ImageFormat format, uint32_t width, uint32_t height)
		{
#if 0
			bool multisampled = samplers > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samplers, format, width, height, GL_FALSE);
			}
			else
			{
				// allocate memory
				//TODO, WHY level COULD NOT BE 0 ?, miplevel must >= 1?
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
#endif
			bool multisampled = samples > 1;
			Ref<Image2D> image;
			if(multisampled)
			{
				
			}
			else
			{
				ImageSpecification spec;
				spec.Format = format;
				spec.Width = width;
				spec.Height = height;
				image = Image2D::Create(spec);
				image->Invalidate();
			}

			Ref<OpenGLImage2D> glImage = image.As<OpenGLImage2D>();
			glImage->CreateSampler(TextureProperties());
			glFramebufferTexture2D(GL_FRAMEBUFFER, Utils::DepthAttachmentType(format), TextureTarget(multisampled), glImage->GetRendererID(), 0);
			return image;
		}
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec), m_Width(spec.Width), m_Height(spec.Height)
	{
		HZ_CORE_ASSERT(spec.Attachments.Attachments.size(), "");
		for (auto specification : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(specification.TextureFormat))
				m_ColorAttachmentFormats.emplace_back(specification.TextureFormat);
			else
				m_DepthAttachmentFormat = specification.TextureFormat;
		}
		uint32_t width = spec.Width;
		uint32_t height = spec.Height;
		if(width == 0)
		{
			width = Application::Get().GetWindow().Width();
			height = Application::Get().GetWindow().Height();
		}
		Resize(width, height, true);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		Ref<OpenGLFramebuffer> instance = this;
		Renderer::Submit([instance]() 
		{
			glDeleteFramebuffers(1, &instance->m_RendererID);
			//TODO, WHY NOT CLEAR COLORATTACHMENT?
		});
	}


	void OpenGLFramebuffer::Bind() const
	{
		Ref<const OpenGLFramebuffer> instance = this;
		Renderer::Submit([instance]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);
			// Handle Imgui's Panel resize, which is not handle by glfw event
			glViewport(0, 0, instance->m_Width, instance->m_Height);
		});
	}

	void OpenGLFramebuffer::UnBind() const
	{
		Renderer::Submit([]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		});
	}
	
	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
	{
		if(!forceRecreate && (m_Width == width && m_Height == height))
			return;

		m_Width = width;
		m_Height = height;
		Ref<OpenGLFramebuffer> instance = this;
		Renderer::Submit([instance]() mutable
		{
			if(instance->m_RendererID)
			{
				glDeleteFramebuffers(1, &instance->m_RendererID);
				instance->m_ColorAttachments.clear();
			}
			glGenFramebuffers(1, &instance->m_RendererID);
			glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);

			if(instance->m_ColorAttachmentFormats.size())
			{
				instance->m_ColorAttachments.resize(instance->m_ColorAttachmentFormats.size());

				for(auto i=0; i< instance->m_ColorAttachmentFormats.size(); ++i)
				{
					instance->m_ColorAttachments[i] = Utils::CreateAndAttachColorAttachment(instance->m_Specification.Samples, instance->m_ColorAttachmentFormats[i],
						instance->m_Width, instance->m_Height, i);
				}

				if(instance->m_DepthAttachmentFormat != ImageFormat::None)
				{
					instance->m_DepthAttachment = Utils::AttachDepthTexture(instance->m_Specification.Samples, instance->m_DepthAttachmentFormat,
						instance->m_Width, instance->m_Height);
				}

				if(instance->m_ColorAttachments.size() > 1)
				{
					HZ_CORE_ASSERT(instance->m_ColorAttachments.size() <= 4, "");
					GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
					glDrawBuffers(instance->m_ColorAttachments.size(), buffers);
				}
				else if(instance->m_ColorAttachments.empty())
				{
					// Only depth buffer
					glDrawBuffer(GL_NONE);
				}

				HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		});
	}

	void OpenGLFramebuffer::AddResizeCallback(const std::function<void(Ref<Framebuffer>)>& func)
	{
		//Empty
	}

	void OpenGLFramebuffer::BindTexture(uint32_t attachmentIndex, uint32_t slot) const
	{
		Ref<const OpenGLFramebuffer> instance = this;
		Renderer::Submit([instance, attachmentIndex, slot]()
		{
			glBindTextureUnit(slot, instance->m_ColorAttachments[attachmentIndex]);
		});
	}
}

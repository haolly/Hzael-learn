#pragma once
#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel
{
	class RenderCommandBuffer;

	class OpenGLRenderer : public RendererAPI
	{
	public:
		~OpenGLRenderer() override = default;
		void Init() override;
		void Shutdown() override;
		void BeginFrame() override;
		void EndFrame() override;
		void BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, const Ref<RenderPass>& renderPass, bool explicitClear) override;
		void EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer) override;
		RendererCapabilities& GetCapabilities() override;
	};
}

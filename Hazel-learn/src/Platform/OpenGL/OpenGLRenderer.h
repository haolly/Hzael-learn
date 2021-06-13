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
		//TODO, 哪里调用的？
		void BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, const Ref<RenderPass>& renderPass, bool explicitClear) override;
		void EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer) override;
		
		RendererCapabilities& GetCapabilities() override;
		void SubmitFullscreenQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline,
			Ref<UniformBufferSet> uniformBufferSet, Ref<Material> material) override;
		void RenderQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<UniformBufferSet> uniformBufferSet,
			Ref<Material> material, const glm::mat4& transform) override;
		void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<UniformBufferSet> uniformBufferSet,
			Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform,
			uint32_t indexCount) override;
	};
}

#pragma once
#include <glm/glm.hpp>

#include "Material.h"
#include "Pipeline.h"
#include "RenderCommandBuffer.h"
#include "UniformBufferSet.h"

namespace Hazel
{
	struct RendererCapabilities;

	enum class RendererAPIType
	{
		None = 0,
		OpenGL = 1,
		Vulkan = 2,
	};

	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, const Ref<RenderPass>& renderPass,
		                             bool explicitClear = false) = 0;
		virtual void EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer) = 0;
		virtual void SubmitFullscreenQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline,
		                                  Ref<UniformBufferSet> uniformBufferSet, Ref<Material> material) = 0;

		virtual void RenderQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline,
		                        Ref<UniformBufferSet> uniformBufferSet, Ref<Material> material, const glm::mat4& transform) = 0;
		virtual void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline,
		                            Ref<UniformBufferSet> uniformBufferSet, Ref<Material> material,
		                            Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform,
		                            uint32_t indexCount = 0) = 0;

		virtual RendererCapabilities& GetCapabilities() = 0;

		static void SetAPI(RendererAPIType api);
		static RendererAPIType GetAPI() { return s_CurrentRendererAPI; }
	private:
		inline static RendererAPIType s_CurrentRendererAPI = RendererAPIType::OpenGL;
	};
}

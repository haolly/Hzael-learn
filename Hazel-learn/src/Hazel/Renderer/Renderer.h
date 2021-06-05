#pragma once
#include "Material.h"
#include "RendererCapabilities.h"
#include "RenderCommandQueue.h"
#include "RendererAPI.h"

namespace Hazel {
	class RenderPass;
	class Pipeline;
	class RenderCommandBuffer;

	struct RendererConfig
	{
		uint32_t FramesInFlight = 3;
	};

	class Renderer 
	{
	public:
		static void Init();
		static void ShutDown();

		static RendererCapabilities& GetCapabilities();

		template<typename FuncT>
		static void Submit(FuncT&& func)
		{
			auto renderCmd = [](void* ptr)
			{
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();
				//todo
				pFunc->~FuncT();
			};

			// Note, why not directly pass func to Allocate? convert stateful lambda into a function pointer
			auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
			// this memory blob will be passed to func when execute render command
			new (storageBuffer) FuncT(std::forward<FuncT>(func));
		}

		static void WaitAndRender();

		static void BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<RenderPass> renderPass, bool explicitClear = false);
		static void EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer);

		static void BeginFrame();
		static void EndFrame();

		static void RenderQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipline, Ref<UniformBufferSet> uniformBufferSet,
								Ref<Material> material, const glm::mat4& transform);
		
		//static void Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPIType GetAPI() { return RendererAPI::GetAPI();}

	private:
		static RenderCommandQueue& GetRenderCommandQueue();
	};

}

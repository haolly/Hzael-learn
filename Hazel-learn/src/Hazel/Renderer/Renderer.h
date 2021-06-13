#pragma once
#include "Material.h"
#include "RendererCapabilities.h"
#include "RenderCommandQueue.h"
#include "RendererAPI.h"

namespace Hazel
{
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
		static Ref<ShaderLibrary> GetShaderLibrary();

		template <typename FuncT>
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
			new(storageBuffer) FuncT(std::forward<FuncT>(func));
		}

		static void WaitAndRender();

		static void BeginRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<RenderPass> renderPass, bool explicitClear = false);
		static void EndRenderPass(Ref<RenderCommandBuffer> renderCommandBuffer);

		static void BeginFrame();
		static void EndFrame();

		static void RenderQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipline, Ref<UniformBufferSet> uniformBufferSet,
		                       Ref<Material> material, const glm::mat4& transform);
		static void RenderGeometry(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<UniformBufferSet> uniformBufferSet,
								Ref<Material> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, const glm::mat4& transform, uint32_t indexCount = 0);

		static void SubmitQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Material> material, const glm::mat4& transform = glm::mat4(1.0f));

		static Ref<Texture2D> GetWhiteTexture();

		static uint32_t GetCurrentFrameIndex();
		static RendererConfig& GetConfig();

		inline static RendererAPIType GetAPI() { return RendererAPI::GetAPI(); }

	private:
		static RenderCommandQueue& GetRenderCommandQueue();
	};

	namespace Utils
	{
		inline void DumpGPUInfo()
		{
			auto& caps = Renderer::GetCapabilities();
			HZ_CORE_INFO("OpenGL Info:");
			HZ_CORE_INFO("	Vendor: {0}", caps.Vendor);
			HZ_CORE_INFO("	Device: {0}", caps.Device);
			HZ_CORE_INFO("	Version: {0}", caps.Version);
		}
	}
}

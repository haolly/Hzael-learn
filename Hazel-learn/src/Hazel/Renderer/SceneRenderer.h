#pragma once
#include "Camera.h"
#include "Material.h"
#include "Pipeline.h"
#include "RenderCommandBuffer.h"
#include "RenderPass.h"
#include "Shader.h"
#include "UniformBufferSet.h"
#include "glm/fwd.hpp"
#include "Hazel/Scene/Scene.h"

namespace Hazel
{
	struct SceneRendererOptions
	{
		bool ShowGrid = true;
		bool ShowBoundingBoxes = false;
		bool ShowSelectedInWireframe = false;
		bool ShowCollidersWireframe = false;
	};

	struct SceneRendererCamera
	{
		Camera Camera;
		glm::mat4 ViewMatrix;
		float Near, Far;
		float FOV;
	};

	struct SceneRendererSpecification
	{
		bool SwapChainTarget = false;
	};

	class SceneRenderer : RefCounted
	{
	public:
		SceneRenderer(Ref<Scene> scene, SceneRendererSpecification specification = SceneRendererSpecification());
		~SceneRenderer();

		void Init();
		void SetScene(Ref<Scene> scene);
		void SetViewportSize(uint32_t width, uint32_t height);

		void BeginScene(const SceneRendererCamera& camera);
		void EndScene();

		Ref<RenderPass> GetFinalRenderPass();
		Ref<RenderPass> GetExternalCompositeRenderPass() { return m_ExternalCompositeRenderPass; }
		Ref<Image2D> GetFinalPassImage();

		SceneRendererOptions& GetOptions();

		void OnImGuiRender();
		static void WaitForThreads();

	private:
		void FlushDrawList();
		void ClearPass();
		void ShadowMapPass();
		void GeometryPass();
		void CompositePass();
		void BloomBlurPass();

		struct CascadeData
		{
			glm::mat4 ViewProj;
			glm::mat4 View;
			float SplitDepth;
		};

	private:
		Ref<Scene> m_Scene;
		SceneRendererSpecification m_Specification;
		Ref<RenderCommandBuffer> m_CommandBuffer;

		struct SceneInfo
		{
			SceneRendererCamera SceneCamera;
		} m_SceneData;

		// Uniform buffer
		struct UBScene
		{
			glm::vec3 u_CameraPosition;
		} SceneDataUB;

		struct UBCamera
		{
			glm::mat4 ViewProjection;
			glm::mat4 InverseViewProjection;
			glm::mat4 View;
		} CameraData;

		struct UBShadow
		{
			glm::mat4 ViewProjection[4];
		} ShadowData;
		
		Ref<Shader> m_CompositeShader;
		Ref<Shader> m_BloomBlurShader;
		Ref<Shader> m_BloomBlendShader;


		Ref<UniformBufferSet> m_UniformBufferSet;


		Ref<Material> CompositeMaterial;
		Ref<Material> m_ShadowPassMaterial;
		

		Ref<Pipeline> m_GeometryPipeline;
		Ref<Pipeline> m_GeometryWireframePipeline;
		Ref<Pipeline> m_CompositePipeline;
		Ref<Pipeline> m_ShadowPassPipeline;

		Ref<RenderPass> m_ExternalCompositeRenderPass;

		// Grid
		Ref<Pipeline> m_GridPipeline;
		Ref<Shader> m_GridShader;
		Ref<Material> m_GridMaterial;
		Ref<Material> m_WireframeMaterial;

		

		SceneRendererOptions m_Options;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		bool m_NeedsResize = false;
		bool m_Active = false;
		bool m_ResourcesCreated = false;
	};
}

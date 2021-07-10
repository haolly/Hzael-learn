#include "hazelPCH.h"
#include "SceneRenderer.h"

#include "glm/gtx/quaternion.hpp"
#include "Hazel/Renderer/Renderer.h"

namespace Hazel
{
	static std::vector<std::thread> s_ThreadPool;

	SceneRenderer::SceneRenderer(Ref<Scene> scene, SceneRendererSpecification specification)
		: m_Scene(scene), m_Specification(specification)
	{
		Init();
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::Init()
	{
		//TODO, HZ_SCOPE_TIMER
		if(m_Specification.SwapChainTarget)
			m_CommandBuffer = RenderCommandBuffer::CreateFromSwapChain("SceneRenderer");
		else
			m_CommandBuffer = RenderCommandBuffer::Create(0, "SceneRenderer");

		uint32_t framesInFlight = Renderer::GetConfig().FramesInFlight;
		m_UniformBufferSet = UniformBufferSet::Create(framesInFlight);
		m_UniformBufferSet->Create(sizeof(UBCamera), 0);
		m_UniformBufferSet->Create(sizeof(UBShadow), 1);
		m_UniformBufferSet->Create(sizeof(UBScene), 2);

		m_CompositeShader = Renderer::GetShaderLibrary()->Get("SceneComposite");
		CompositeMaterial = Material::Create(m_CompositeShader);


		// Geometry
		{
			FramebufferSpecification geoFramebufferSpec;
			geoFramebufferSpec.Attachments = {
				FramebufferTextureSpecification(ImageFormat::RGBA32F),
				FramebufferTextureSpecification(ImageFormat::RGBA32F),
				FramebufferTextureSpecification(ImageFormat::Depth),
			};

			Ref<Framebuffer> framebuffer = Framebuffer::Create(geoFramebufferSpec);

			PipelineSpecification pipelineSpecification;
			pipelineSpecification.Layout = {
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float3, "a_Normal"},
				{ShaderDataType::Float3, "a_Tangent"},
				{ShaderDataType::Float3, "a_Binormal"},
				{ShaderDataType::Float2, "a_TexCoord"},
			};
			pipelineSpecification.Shader = Renderer::GetShaderLibrary()->Get("HazelPBR_Static");

			RenderPassSpecification renderPassSpec;
			renderPassSpec.TargetFramebuffer = framebuffer;
			renderPassSpec.DebugName = "Geometry";
			pipelineSpecification.DebugName = "PBR_Static";
			pipelineSpecification.RenderPass = RenderPass::Create(renderPassSpec);
			m_GeometryPipeline = Pipeline::Create(pipelineSpecification);

			pipelineSpecification.Wireframe = true;
			pipelineSpecification.DepthTest = false;
			pipelineSpecification.LineWidth = 2.0f;
			pipelineSpecification.Shader = Renderer::GetShaderLibrary()->Get("Wireframe");
			pipelineSpecification.DebugName = "Wireframe";
			m_GeometryWireframePipeline = Pipeline::Create(pipelineSpecification);
		}


		// External compositing, used by Renderer2D
		if(!m_Specification.SwapChainTarget)
		{
			FramebufferSpecification extCompFramebufferSpec;
			extCompFramebufferSpec.Attachments = {FramebufferTextureSpecification(ImageFormat::RGBA), FramebufferTextureSpecification(ImageFormat::Depth)};
			extCompFramebufferSpec.ClearColor = {0.5f, 0.1f, 0.1f, 1.0f};
			extCompFramebufferSpec.ClearOnLoad = false;

			extCompFramebufferSpec.ExistingImages[0] = m_CompositePipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->GetImage();
			extCompFramebufferSpec.ExistingImages[1] = m_GeometryPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->GetDepthImage();

			Ref<Framebuffer> framebuffer = Framebuffer::Create(extCompFramebufferSpec);

			RenderPassSpecification renderPassSpec;
			renderPassSpec.TargetFramebuffer = framebuffer;
			renderPassSpec.DebugName = "External-Composite";
			m_ExternalCompositeRenderPass = RenderPass::Create(renderPassSpec);
		}

		// Grid
		{
			m_GridShader = Renderer::GetShaderLibrary()->Get("Grid");
			const float gridScale = 16.025f;
			const float gridSize = 0.025f;
			m_GridMaterial = Material::Create(m_GridShader);
			m_GridMaterial->Set("u_Settings.Scale", gridScale);
			m_GridMaterial->Set("u_Settings.Size", gridSize);

			PipelineSpecification pipelineSpec;
			pipelineSpec.DebugName = "Grid";
			pipelineSpec.Shader = m_GridShader;
			pipelineSpec.BackfaceCulling = false;
			pipelineSpec.Layout = {
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float2, "a_TexCoord"},
			};
			pipelineSpec.RenderPass = m_GeometryPipeline->GetSpecification().RenderPass;
			m_GridPipeline = Pipeline::Create(pipelineSpec);
		}


		Ref<SceneRenderer> instance = this;
		Renderer::Submit([instance]() mutable
		{
			instance->m_ResourcesCreated = true;
		});
	}

	void SceneRenderer::SetScene(Ref<Scene> scene)
	{
		HZ_CORE_ASSERT(!m_Active, "Can't cange scenes while rendering");
		m_Scene = scene;
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		if(m_ViewportWidth != width || m_ViewportHeight != height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;
			m_NeedsResize = true;
		}
	}

	void SceneRenderer::BeginScene(const SceneRendererCamera& camera)
	{
		HZ_CORE_ASSERT(m_Scene);
		HZ_CORE_ASSERT(!m_Active);
		m_Active = true;

		if(!m_ResourcesCreated)
			return;

		m_SceneData.SceneCamera = camera;

		if(m_NeedsResize)
		{
			m_GeometryPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->Resize(m_ViewportWidth, m_ViewportHeight);
			if(m_ExternalCompositeRenderPass)
				m_ExternalCompositeRenderPass->GetSpecification().TargetFramebuffer->Resize(m_ViewportWidth, m_ViewportHeight);

			m_NeedsResize = false;
		}

		// update uniform buffer
		UBCamera& cameraData = CameraData;

		auto& sceneCamera = m_SceneData.SceneCamera;
		auto viewProjection = sceneCamera.Camera.GetProjection() * sceneCamera.ViewMatrix;
		//TODO
		glm::vec3 cameraPosition = glm::inverse(sceneCamera.ViewMatrix)[3];

		auto inverseVP = glm::inverse(viewProjection);
		cameraData.ViewProjection = viewProjection;
		cameraData.InverseViewProjection = inverseVP;
		cameraData.View = sceneCamera.ViewMatrix;
		Ref<SceneRenderer> instance = this;
		Renderer::Submit([instance, cameraData]() mutable
		{
			const uint32_t bufferIndex = Renderer::GetCurrentFrameIndex();
			// Camera binding is 0
			instance->m_UniformBufferSet->Get(0, 0, bufferIndex)->RT_SetData(&cameraData, sizeof(cameraData));
		});

		UBScene& sceneData = SceneDataUB;
		sceneData.u_CameraPosition = cameraPosition;
		Renderer::Submit([instance, sceneData]() mutable
		{
			const uint32_t bufferIndex = Renderer::GetCurrentFrameIndex();
			instance->m_UniformBufferSet->Get(2, 0, bufferIndex)->RT_SetData(&sceneData, sizeof(sceneData));
		});
	}

	void SceneRenderer::EndScene()
	{
		HZ_CORE_ASSERT(m_Active);
		FlushDrawList();
		m_Active = false;
	}

	Ref<RenderPass> SceneRenderer::GetFinalRenderPass()
	{
		//todo, what is final ?
		return m_CompositePipeline->GetSpecification().RenderPass;
	}

	Ref<Image2D> SceneRenderer::GetFinalPassImage()
	{
		if(!m_ResourcesCreated)
			return nullptr;
		return m_CompositePipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer->GetImage();
	}

	SceneRendererOptions& SceneRenderer::GetOptions()
	{
		return m_Options;
	}

	void SceneRenderer::OnImGuiRender()
	{
	}

	void SceneRenderer::WaitForThreads()
	{
	}

	void SceneRenderer::FlushDrawList()
	{
		if(m_ResourcesCreated)
		{
			m_CommandBuffer->Begin();
			ShadowMapPass();
			GeometryPass();
			CompositePass();
			m_CommandBuffer->End();
			m_CommandBuffer->Submit();
		}
		else
		{
			m_CommandBuffer->Begin();
			ClearPass();
			m_CommandBuffer->End();
			m_CommandBuffer->Submit();
		}

		
		m_SceneData = {};
	}

	void SceneRenderer::ClearPass()
	{
		Renderer::BeginRenderPass(m_CommandBuffer, m_CompositePipeline->GetSpecification().RenderPass, true);
		Renderer::EndRenderPass(m_CommandBuffer);
	}

	void SceneRenderer::ShadowMapPass()
	{
	}

	void SceneRenderer::GeometryPass()
	{
		Renderer::BeginRenderPass(m_CommandBuffer, m_GeometryPipeline->GetSpecification().RenderPass);

		// Grid
		if(m_Options.ShowGrid)
		{
			const glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
				glm::scale(glm::mat4(1.0f), glm::vec3(8.0f));
			Renderer::RenderQuad(m_CommandBuffer, m_GridPipeline, m_UniformBufferSet, m_GridMaterial, transform);
		}

		Renderer::EndRenderPass(m_CommandBuffer);
	}

	void SceneRenderer::CompositePass()
	{
	}

	void SceneRenderer::BloomBlurPass()
	{
	}
}

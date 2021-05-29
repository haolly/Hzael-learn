#include "hazelPCH.h"

#include "Renderer.h"


#include "Renderer2D.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		// TODO, camera, environment, shader uniforms
		// Copy this because the data will be in the separate thread
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform)
	{
		shader->Bind();
		// TODO viewProjection do not need to upload every frame, but upload per shader per scene
		shader.As<OpenGLShader>()->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		shader.As<OpenGLShader>()->UploadUniformMat4("u_Transform", transform);
		// Bind should not in RenderCommand
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}

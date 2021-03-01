#include "hazelPCH.h"

#include "Renderer.h"

namespace Hazel {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		// TODO, camera, environment, shader uniforms
		// Copy this because the data will be in the separate thread
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		// Bind should not in RenderCommand
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}
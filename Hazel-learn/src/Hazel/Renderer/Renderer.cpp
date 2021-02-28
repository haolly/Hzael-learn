#include "hazelPCH.h"

#include "Renderer.h"

namespace Hazel {

	void Renderer::BeginScene()
	{
		// TODO, camera, environment, shader uniforms
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		// Bind should not in RenderCommand
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}
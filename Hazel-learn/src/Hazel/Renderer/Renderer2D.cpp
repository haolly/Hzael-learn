#include "hazelPCH.h"
#include "Renderer2D.h"

#include "Buffer.h"
#include "OrthographicCamera.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "glm/gtx/transform.hpp"

namespace Hazel {

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
	};

	//Note, 自己管理生命周期来释放GL 资源
	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNC();
		s_Data = new Renderer2DStorage();
		s_Data->QuadVertexArray = VertexArray::Create();

		float squareVertices[5*4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,	1.0f, 0.0f,
			0.5f, 0.5f, 0.0f,	 1.0f, 1.0f,
			-0.5f, 0.5f, 0.0f,	 0.0f, 1.0f,
		};
		
		Ref<VertexBuffer> m_SquareVB;
		m_SquareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)) );
		m_SquareVB->Bind();

		BufferLayout squareLayout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"},
		};
		m_SquareVB->SetLayout(squareLayout);
		s_Data->QuadVertexArray->AddVertexBuffer(m_SquareVB);

		uint32_t squareIndices[3 * 2] = {0, 1, 2, 2, 3, 0};

		Ref<IndexBuffer> m_SquareIB;
		m_SquareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(squareIndices[1])));
		m_SquareIB->Bind();
		s_Data->QuadVertexArray->SetIndexBuffer(m_SquareIB);

		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetInt("u_Texture", 0);
		s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::EndScene()
	{
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->TextureShader->SetFloat4("u_Color", color);
		// 当不需要 texture 的时候，bind 一个White texture, 需要显示 texture 的时候，bind 特定的 texture
		s_Data->WhiteTexture->Bind(0);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * 
			glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
		s_Data->TextureShader->SetMat4("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, texture);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		s_Data->TextureShader->SetFloat4("u_Color", glm::vec4(1.0f));
		texture->Bind(0);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * 
			glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
		s_Data->TextureShader->SetMat4("u_Transform", transform);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray);

	}
}


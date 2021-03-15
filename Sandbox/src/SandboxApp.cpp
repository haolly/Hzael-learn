#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>



#include "Sandbox2D.h"
// temporary
#include "Platform/OpenGL/OpenGLShader.h"

glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_CameraController(1920.0f / 1080.0f, true), m_SquarePosition(0.0f)
	{
		auto cam = camera(5.0f, {1.0f, 2.0f});

		m_VertexArray = Hazel::VertexArray::Create();
		// 默认是在clip space 中，[-1, 1] 的范围
		float vertices[3*7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		};


		Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)) );
		m_VertexBuffer->Bind();

		Hazel::BufferLayout layout = {
			{Hazel::ShaderDataType::Float3, "a_Position"},
			{Hazel::ShaderDataType::Float4, "a_Color"}
		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);



		uint32_t indices[3] = {0, 1, 2};

		Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(indices[1])));
		m_IndexBuffer->Bind();
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSrc = R"(
			#version 410 core 
			layout(location=0) in vec3 a_Position;
			layout(location=1) in vec4 a_Color;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
		std::string fragmentSrc = R"(
			#version 410 core
			layout(location=0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}

		)";
		m_Shader = Hazel::Shader::Create("VertexColorTriangle", vertexSrc, fragmentSrc);

		// Square
		m_SquareVA = Hazel::VertexArray::Create();

		float squareVertices[5*4] = {
			-0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,	 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f,	 1.0f, 1.0f,
			-0.5f, 0.5f, 0.0f,	 0.0f, 1.0f,
		};
		
		Hazel::Ref<Hazel::VertexBuffer> m_SquareVB;
		m_SquareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)) );
		m_SquareVB->Bind();

		Hazel::BufferLayout squareLayout = {
			{Hazel::ShaderDataType::Float3, "a_Position"},
			{Hazel::ShaderDataType::Float2, "a_TexCoord"},
		};
		m_SquareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(m_SquareVB);

		uint32_t squareIndices[3 * 2] = {0, 1, 2, 2, 3, 0};

		Hazel::Ref<Hazel::IndexBuffer> m_SquareIB;
		m_SquareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(squareIndices[1])));
		m_SquareIB->Bind();
		m_SquareVA->SetIndexBuffer(m_SquareIB);

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec3 v_Position;
			uniform vec3 u_Color;
			void main()
			{
				color = vec4(u_Color, 1.0);
			}

		)";

		m_FlatColorShader = Hazel::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		const auto textureShader = shaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();

		// TODO 这里的slot 在material中映射
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(float deltaTime) override
	{
		// Update
		m_CameraController.OnUpdate(deltaTime);

		// Render
		Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
		Hazel::RenderCommand::Clear();

		Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		const auto textureShader = shaderLibrary.Get("Texture");	
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();

		for(int y=0; y <20; ++y)
		{
			for(int x= 0; x < 20; ++x)
			{
				// square with is 1, so scale 0.1 is 0.1, so x*0.11 will grid this
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0);
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Hazel::Renderer::Submit(m_SquareVA, m_FlatColorShader, transform);
			}
		}

		m_Texture->Bind(0);
		Hazel::Renderer::Submit(m_SquareVA, textureShader, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_LogoTexture->Bind();
		Hazel::Renderer::Submit(m_SquareVA, textureShader, 
		glm::translate(glm::mat4(1.0f), glm::vec3(0.25f, -0.25f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//Hazel::Renderer::Submit(m_VertexArray, m_Shader);
		
		Hazel::Renderer::EndScene();
	}

	void OnImGuiRenderer() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}
	
	void OnEvent(Hazel::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

private:
	Hazel::ShaderLibrary shaderLibrary;
	Hazel::OrthographicCameraController m_CameraController;
	glm::vec3 m_SquarePosition;

	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;

	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	
	Hazel::Ref<Hazel::Texture2D> m_Texture, m_LogoTexture;

	glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.78f};
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
		
	}
};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}

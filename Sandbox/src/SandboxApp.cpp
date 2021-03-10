#include <Hazel.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

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
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquarePosition(0.0f)
	{
		auto cam = camera(5.0f, {1.0f, 2.0f});

		m_VertexArray.reset(Hazel::VertexArray::Create());
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
			#version 330 core
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
			#version 330 core
			layout(location=0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}

		)";
		m_Shader.reset(Hazel::Shader::Create(vertexSrc, fragmentSrc));

		// Square
		m_SquareVA.reset(Hazel::VertexArray::Create());

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

		m_FlatColorShader.reset(Hazel::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

		std::string textureColorShaderVertexSrc = R"(
			#version 330 core
			layout(location=0) in vec3 a_Position;
			layout(location=1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;
			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
		std::string textureColorShaderFragmentSrc = R"(
			#version 330 core
			layout(location=0) out vec4 color;
			in vec2 v_TexCoord;
			uniform sampler2D u_Texture;
			void main()
			{
				color = texture(u_Texture, v_TexCoord);
			}

		)";

		m_TextureShader.reset(Hazel::Shader::Create(textureColorShaderVertexSrc, textureColorShaderFragmentSrc));

		m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)->Bind();

		// TODO 这里的slot 在material中映射
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(float deltaTime) override
	{
		HZ_INFO("deltaTime: {0}", deltaTime);
		if(Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * deltaTime;
		else if(Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * deltaTime;

		if(Hazel::Input::IsKeyPressed(HZ_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * deltaTime;
		else if(Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * deltaTime;

		
		if(Hazel::Input::IsKeyPressed(HZ_KEY_J))
			m_SquarePosition.x -= m_CameraMoveSpeed * deltaTime;
		else if(Hazel::Input::IsKeyPressed(HZ_KEY_L))
			m_SquarePosition.x += m_CameraMoveSpeed * deltaTime;

		if(Hazel::Input::IsKeyPressed(HZ_KEY_I))
			m_SquarePosition.y += m_CameraMoveSpeed * deltaTime;
		else if(Hazel::Input::IsKeyPressed(HZ_KEY_K))
			m_SquarePosition.y -= m_CameraMoveSpeed * deltaTime;

		if(Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_CameraRotation += m_CameraRotateSpeed * deltaTime;
		else if(Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_CameraRotation -= m_CameraRotateSpeed * deltaTime;
		
		Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Hazel::Renderer::BeginScene(m_Camera);

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)->Bind();

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
		Hazel::Renderer::Submit(m_SquareVA, m_TextureShader, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_LogoTexture->Bind();
		Hazel::Renderer::Submit(m_SquareVA, m_TextureShader, 
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
		Hazel::EventDispatcher dispatcher(e);
	}

private:
	Hazel::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	glm::vec3 m_SquarePosition;
	float m_CameraRotation = 0.0f;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotateSpeed = 60.0f;

	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;

	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	
	Hazel::Ref<Hazel::Shader> m_TextureShader;
	Hazel::Ref<Hazel::Texture2D> m_Texture, m_LogoTexture;

	glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.78f};
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{
		
	}
};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}

#include <Hazel.h>

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		:Layer("Example"){}
	void OnUpdate() override
	{
		//HZ_INFO("Example layer OnUpdate");
		if(Hazel::Input::IsKeyPressed(HZ_KEY_A))
		{
			HZ_INFO("A Key pressed");
		}
	}
	void OnEvent(Hazel::Event& e) override
	{
		//HZ_TRACE("Example layer event: {0}", e);
	}
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Hazel::ImGuiLayer());
	}

	~Sandbox()
	{
		
	}
};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}

#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv)
{
	Hazel::Log::Init();
	HZ_PROFILE_BEGIN_SESSION("Startup", "Hazel-profile-startup.json");
	auto app = Hazel::CreateApplication();
	HZ_PROFILE_END_SESSION();
	HZ_PROFILE_BEGIN_SESSION("Runtime", "Hazel-profile-runtime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();
	HZ_PROFILE_BEGIN_SESSION("Shutdown", "Hazel-profile-shutdown.json");
	delete app;
	HZ_PROFILE_END_SESSION();
}
#endif // HZ_PLATFORM_WINDOWS

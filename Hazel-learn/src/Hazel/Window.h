#pragma once
#include "hazelPCH.h"
#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		WindowProps(const std::string& title="Hazel-learn", const unsigned int width=1280, const unsigned int height=720)
			: Title(title), Width(width), Height(height) {}
	};

	// Interface representing a desktop system based window, while mobile has no window but a surface
	class HAZEL_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window() {}

		virtual void OnUpdate() = 0;
		virtual unsigned int Width() const = 0;
		virtual unsigned int Height() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	private:
		
	};

}

#pragma once
#include <GLFW/glfw3.h>
#include "Hazel/Core/Window.h"

namespace Hazel
{
    class WindowsWindow :
        public Window
    {
    public:
	    explicit WindowsWindow(const WindowProps& props);
	    ~WindowsWindow() override;

        unsigned Width() const override { return m_Data.Width; }
        unsigned Height() const override { return m_Data.Height; }
        void SetEventCallback(const EventCallbackFn& callback) override
        {
            m_Data.EventCallbackFn = callback;
        }
        bool IsVSync() const override;
	    void OnUpdate() override;
	    void SetVSync(bool enabled) override;

	    inline void* GetNativeWindow() const override
	    {
		    return m_Window;
	    }
    private:
        virtual void Init(const WindowProps& props);
        virtual void ShutDown();
    private:
        GLFWwindow* m_Window;

    	struct WindowData
    	{
            std::string Title;
            unsigned int Width, Height;
            bool VSync;

            EventCallbackFn EventCallbackFn;
    	};

        WindowData m_Data;
    };
}

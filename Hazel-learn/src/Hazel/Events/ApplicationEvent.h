#pragma once
#include "Event.h"

namespace Hazel
{
	class HAZEL_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t widht, uint32_t height)
			: m_Width(widht), m_Height(height) {}
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
			EVENT_CLASS_TYPE(WindowResize)
			std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }
	private:
		uint32_t m_Width;
		uint32_t m_Height;
	};

	class HAZEL_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
			EVENT_CLASS_TYPE(WindowClose)
			std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowCloseEvent";
			return ss.str();
		}
	};
}

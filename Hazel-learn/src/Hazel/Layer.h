#pragma once
#include "Hazel/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel
{
	class HAZEL_API Layer
	{
	public:
		explicit Layer(const std::string& name = "Layer");
		virtual ~Layer();
		
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnImGuiRenderer() {} 
		virtual void OnEvent(Event& e) {}  
		inline const std::string GetName() const { return m_DebugName;}
	protected:
		std::string m_DebugName;
	};
}


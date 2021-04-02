#pragma once
#include "Hazel/Core/Layer.h"

namespace Hazel
{
	class HAZEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer() override;

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRenderer() override;

		void Begin();
		void End();
		void OnEvent(Event& e) override;

		void BlockEvents(bool block) {m_BlockEvents = block;}

	private:
		bool m_BlockEvents = true;
	};
}

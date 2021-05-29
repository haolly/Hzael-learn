#pragma once
#include "Layer.h"
#include "Hazel/Core/Base.h"

namespace Hazel
{
	class HAZEL_API LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

		std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
		std::vector<Layer*>::const_iterator end()  const { return m_Layers.end(); }
	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}

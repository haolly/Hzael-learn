#pragma once
#include <string>

namespace Hazel
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& framentSrc);
		~Shader();
		void Bind() const;
		void UnBind() const;
	private:
		uint32_t m_RenderID;
	};
}

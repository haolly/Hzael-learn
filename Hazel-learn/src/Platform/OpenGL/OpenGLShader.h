#pragma once
#include "Hazel/Renderer/Shader.h"

namespace Hazel {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& framentSrc);
		~OpenGLShader();
		void Bind() const override;
		void UnBind() const override;
	private:
		uint32_t m_RenderID;
	};
}


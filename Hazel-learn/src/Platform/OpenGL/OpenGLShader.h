#pragma once
#include "Hazel/Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Hazel {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& framentSrc);
		~OpenGLShader();
		void Bind() const override;
		void UnBind() const override;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const override;
	private:
		uint32_t m_RenderID;
	};
}


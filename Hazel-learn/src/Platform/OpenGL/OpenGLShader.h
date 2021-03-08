#pragma once
#include "Hazel/Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Hazel {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& framentSrc);
		virtual ~OpenGLShader();
		void Bind() const override;
		void UnBind() const override;

		void UploadUniformInt(const std::string& name, int value) const;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;


		void UploadUniformFloat(const std::string& name, float value) const;
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value) const;
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value) const;
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value) const;

	private:
		uint32_t m_RenderID;
	};
}


#pragma once
#include "Hazel/Renderer/Shader.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Hazel {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& framentSrc);
		explicit OpenGLShader(const std::string& filepath);
		virtual ~OpenGLShader();
		void Bind() const override;
		void UnBind() const override;

		const std::string& GetName() const override { return m_Name; }

		void UploadUniformInt(const std::string& name, int value) const;
		void UploadUniformIntArray(const std::string& name, int* value, uint32_t count) const;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;


		void UploadUniformFloat(const std::string& name, float value) const;
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value) const;
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value) const;
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value) const;

		void SetFloat(const std::string& name, float value) override;
		void SetFloat3(const std::string& name, const glm::vec3& value) override;
		void SetFloat4(const std::string& name, const glm::vec4& value) override;

		void SetMat4(const std::string& name, const glm::mat4& value) override;

	private:
		std::string ReadFile(const std::string& filepath) const;
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& shaderSource);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

	public:
		void SetInt(const std::string& name, int value) override;
		void SetIntArray(const std::string& name, int* value, uint32_t count) override;
	private:
		uint32_t m_RenderID;
		std::string m_Name;
	};
}


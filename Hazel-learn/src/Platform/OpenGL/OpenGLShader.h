#pragma once
#include "Hazel/Renderer/Shader.h"
#include <glm/glm.hpp>

#include <glad/glad.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Hazel {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader() = default;
		explicit OpenGLShader(const std::string& filepath, bool forceRecompile);
		static Ref<OpenGLShader> CreateFromString(const std::string& source);
		
		void Reload(bool forceCompile) override;
		size_t GetHash() const override;
		uint32_t GetRendererID() const { return m_RenderID;}
		void Bind() const override;
		
		void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;
		const std::unordered_map<std::string, ShaderBuffer>& GetShaderBuffers() const override;
		const std::unordered_map<std::string, ShaderResourceDeclaration>& GetResources() const override;
		
		const std::string& GetName() const override { return m_Name; }
		
		static void ClearUniformBuffers();

		void SetUniformBuffer(const std::string& name, const void* data, uint32_t size);	
		void SetUniform(const std::string& name, int value) const;
		void SetUniform(const std::string& name, float value) const;
		void SetUniform(const std::string& name, uint32_t value) const;
		void SetUniform(const std::string& name, const glm::vec2& value) const;
		void SetUniform(const std::string& name, const glm::vec3& value) const;
		void SetUniform(const std::string& name, const glm::vec4& value) const;
		void SetUniform(const std::string& name, const glm::mat4& matrix) const;
		void SetUniform(const std::string& name, const glm::mat3& matrix) const;
	
		void UploadUniformInt(uint32_t location, int value) const;
		void UploadUniformIntArray(uint32_t location, int* value, uint32_t count) const;
		void UploadUniformFloat(uint32_t location, float value) const;
		void UploadUniformFloat2(uint32_t location, const glm::vec2& value) const;
		void UploadUniformFloat3(uint32_t location, const glm::vec3& value) const;
		void UploadUniformFloat4(uint32_t location, const glm::vec4& value) const;
		void UploadUniformMat4(uint32_t location, const glm::mat4& matrix) const;
		void UploadUniformMat3(uint32_t location, const glm::mat3& matrix) const;

		void UploadUniformInt(const std::string& name, int value) const;
		void UploadUniformIntArray(const std::string& name, int* value, uint32_t count) const;
		void UploadUniformFloat(const std::string& name, float value) const;
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value) const;
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value) const;
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value) const;
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;

	private:
		void Load(const std::string& source, bool forceCompile);
		void Reflect(std::vector<uint32_t>& shaderData);
		std::string ReadShaderFromFile(const std::string& filepath) const;
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

		void CompileOrGetVulkanBinaries(std::unordered_map<uint32_t, std::vector<uint32_t>>& outputBinary, bool forceCompile);
		void CompileOrGetOpenGLBinaries(const std::unordered_map<uint32_t, std::vector<uint32_t>>& vulkanBinaries, bool forceCompile);
		void ParseConstantBuffers(const spirv_cross::CompilerGLSL& compiler);

		int32_t GetUniformLocation(const std::string& name) const;
	
	private:
		uint32_t m_RenderID;
		bool m_Loaded = false;
		bool m_IsComplete = false;

		//TODO?
		uint32_t m_ConstantBufferOffset = 0;
		std::string m_Name;
		std::string m_AssetPath;
		
		std::unordered_map<GLenum, std::string> m_ShaderSource;
		std::vector<ShaderReloadedCallback> m_ShaderReloadedCallbacks;
		
		//Note, all shaders in a program share the same uniform buffer,
		//ref: https://www.lighthouse3d.com/tutorials/glsl-tutorial/uniform-blocks/
		inline static std::unordered_map<uint32_t, ShaderUniformBuffer> s_UniformBuffers;

		// todo, push constant in vulkan, 一个shader 可以有多个const buffer
		std::unordered_map<std::string, ShaderBuffer> m_Buffers;
		std::unordered_map<std::string, ShaderResourceDeclaration> m_Resources;
		std::unordered_map<std::string, GLint> m_UniformLocations;
	};
}


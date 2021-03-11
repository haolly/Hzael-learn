#include "hazelPCH.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>

namespace Hazel {

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		HZ_CORE_ASSERT(false, "Unknow shader type:{0}", type);
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
	}

	OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& framentSrc)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = framentSrc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RenderID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RenderID);
	}

	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value) const
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value) const
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value) const
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value) const
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values) const
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const
	{
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}


	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const
	{
		// Usually, the shader is pre-parsed to get the uniforms and locations
		// Uniforms groups to TWO kind, One kind is set by Renderer, like light direction, viewProjection etc.
		// the other kind is set by Material, like color
		GLint location = glGetUniformLocation(m_RenderID, name.c_str());
		// If we use a row-major matrix, we should transport this matrix, IE pass 3rd argument to GL_TRUE 
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	
	std::string OpenGLShader::ReadFile(const std::string& filepath) const
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in, std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			HZ_CORE_ERROR("Could not open file: {0}", filepath);
		}
		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t typeTokenPos = source.find(typeToken, 0);
		while (typeTokenPos != std::string::npos)
		{
			// find the first pos of ANY character in "\r\n"
			size_t eolOfToken = source.find_first_of("\r\n", typeTokenPos);
			HZ_CORE_ASSERT(eolOfToken != std::string::npos, "Syntax error");
			size_t typeBegin = typeTokenPos + typeTokenLength + 1;
			std::string type = source.substr(typeBegin, eolOfToken - typeBegin);
			HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specification");

			//normal shader start
			//size_t rawShaderStart = source.find_first_of("\r\n", eolOfToken);
			size_t rawShaderStart = eolOfToken + 1;
			HZ_CORE_ASSERT(rawShaderStart != std::string::npos, "Syntax error");
			typeTokenPos = source.find(typeToken, rawShaderStart);
			size_t count = rawShaderStart;
			const std::string& rawSource = source.substr(rawShaderStart, typeTokenPos - count);
			shaderSources[ShaderTypeFromString(type)] = rawSource;
		}
		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		std::unordered_map<GLenum, GLuint> glShaderIDs(shaderSources.size());
		for (auto kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar *source_c = source.c_str();
			glShaderSource(shader, 1, &source_c, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if(isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
				
				// We don't need the shader anymore.
				if(type == GL_FRAGMENT_SHADER)
				{
					glDeleteShader(shader);
					glDeleteShader(glShaderIDs[GL_VERTEX_SHADER]);
				}
				else
				{
					glDeleteShader(shader);
				}
				glDeleteProgram(program);

				HZ_CORE_ERROR("{0}", infoLog.data());
				HZ_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}
			glShaderIDs[type] = shader;
			glAttachShader(program, shader);
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			
			// We don't need the program anymore.
			glDeleteProgram(program);

			for (auto kv : glShaderIDs)
			{
				glDeleteShader(kv.second);
			}

			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_CORE_ASSERT(false, "Link shader programmer failure");
			return;
		}

		for (auto kv : glShaderIDs)
		{
			glDetachShader(program, kv.second);
		}
		m_RenderID = program;
	}
}

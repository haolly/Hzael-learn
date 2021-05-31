#include "hazelPCH.h"
#include "OpenGLShader.h"


#include <filesystem>
#include <glad/glad.h>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include "Hazel/Renderer/Renderer.h"

namespace Hazel
{
	namespace Utils
	{
		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;
			HZ_CORE_ASSERT(false, "Unknow shader type:{0}", type);
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:
				return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER:
				return shaderc_glsl_fragment_shader;
			}
			HZ_CORE_ASSERT(false, "");
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:
				return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER:
				return "GL_FRAGMENT_SHADER";
			}
			HZ_CORE_ASSERT(false, "");
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if(!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:
				return ".cached_opengl.vert"; 
			case GL_FRAGMENT_SHADER:
				return ".cached_opengl.frag";
			}
			HZ_CORE_ASSERT(false, "");
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:
				return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:
				return ".cached_vulkan.frag";
			}
			HZ_CORE_ASSERT(false, "");
			return "";
		}

		static const char* GLShaderTypeToString(uint32_t stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:
					return "Vertex";
				case GL_FRAGMENT_SHADER:
					return "Fragment";
				case GL_COMPUTE_SHADER:
					return "Compute";
			}
			HZ_CORE_ASSERT(false, "");
			return "";
		}

		static ShaderUniformType SPIRTypeToShaderUniformType(spirv_cross::SPIRType type)
		{
			switch (type.basetype)
			{
				case spirv_cross::SPIRType::Boolean:
					return ShaderUniformType::Bool;
				case spirv_cross::SPIRType::Int:
					return ShaderUniformType::Int;
				case spirv_cross::SPIRType::UInt:
					return ShaderUniformType::UInt;
				case spirv_cross::SPIRType::Float:
					{
						if(type.vecsize == 1)
							return ShaderUniformType::Float;
						if(type.vecsize == 2)
							return ShaderUniformType::Vec2;
						if(type.vecsize == 3)
							return ShaderUniformType::Vec3;
						if(type.vecsize == 4)
							return ShaderUniformType::Vec4;
					}
				default: ;
			}
			HZ_CORE_ASSERT(false, "Unknow type");
			return ShaderUniformType::None;
		}
	}

	OpenGLShader::OpenGLShader(const std::string& filepath, bool forceRecompile)
		: m_AssetPath(filepath)
	{
		size_t found = filepath.find_last_of("/\\");
		m_Name = found != std::string::npos ? filepath.substr(found + 1) : filepath;
		found = m_Name.find_last_of(".");
		m_Name = found != std::string::npos ? m_Name.substr(0, found) : m_Name;
		Reload(forceRecompile);
	}

	Ref<OpenGLShader> OpenGLShader::CreateFromString(const std::string& source)
	{
		Ref<OpenGLShader> shader = Ref<OpenGLShader>::Create();
		shader->Load(source, true);
		return shader;
	}

	void OpenGLShader::Reload(bool forceCompile)
	{
		std::string source = ReadShaderFromFile(m_AssetPath);
		Load(source, forceCompile);
	}
	
	void OpenGLShader::Load(const std::string& source, bool forceCompile)
	{
		m_ShaderSource = PreProcess(source);
		Utils::CreateCacheDirectoryIfNeeded();
		//BUG!, 不要在构造函数的调用链条里面使用 Ref this，这会导致this 被delete掉
		//Ref<OpenGLShader> instance = this;
		std::array<std::vector<uint32_t>, 2> vulkanBinaries;
		std::unordered_map<uint32_t, std::vector<uint32_t>> shaderData;
		this->CompileOrGetVulkanBinaries(shaderData, forceCompile);
		this->CompileOrGetOpenGLBinaries(shaderData, forceCompile);
		
		// Renderer::Submit([instance, forceCompile]() mutable
		// {
		// 	std::array<std::vector<uint32_t>, 2> vulkanBinaries;
		// 	std::unordered_map<uint32_t, std::vector<uint32_t>> shaderData;
		// 	instance->CompileOrGetVulkanBinaries(shaderData, forceCompile);
		// 	instance->CompileOrGetOpenGLBinaries(shaderData, forceCompile);
		// });
	}
	
	void OpenGLShader::ClearUniformBuffers()
	{
		s_UniformBuffers.clear();
	}

	void OpenGLShader::SetUniformBuffer(const std::string& name, const void* data, uint32_t size)
	{
		//todo
	}

	void OpenGLShader::SetUniform(const std::string& name, int value) const
	{
		HZ_CORE_ASSERT(m_UniformLocations.find(name) != m_UniformLocations.end(), "");
		const GLint location = m_UniformLocations.at(name);
		glUniform1i(location, value);
	}

	void OpenGLShader::SetUniform(const std::string& name, float value) const
	{
		HZ_CORE_ASSERT(m_UniformLocations.find(name) != m_UniformLocations.end(), "");
		const GLint location = m_UniformLocations.at(name);
		glUniform1f(location, value);
	}

	void OpenGLShader::SetUniform(const std::string& name, uint32_t value) const
	{
		HZ_CORE_ASSERT(m_UniformLocations.find(name) != m_UniformLocations.end(), "");
		const GLint location = m_UniformLocations.at(name);
		glUniform1ui(location, value);
	}

	void OpenGLShader::SetUniform(const std::string& name, const glm::vec2& value) const
	{
		HZ_CORE_ASSERT(m_UniformLocations.find(name) != m_UniformLocations.end(), "");
		const GLint location = m_UniformLocations.at(name);
		glUniform2fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetUniform(const std::string& name, const glm::vec3& value) const
	{
		HZ_CORE_ASSERT(m_UniformLocations.find(name) != m_UniformLocations.end(), "");
		const GLint location = m_UniformLocations.at(name);
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetUniform(const std::string& name, const glm::vec4& value) const
	{
		HZ_CORE_ASSERT(m_UniformLocations.find(name) != m_UniformLocations.end(), "");
		const GLint location = m_UniformLocations.at(name);
		glUniform4fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetUniform(const std::string& name, const glm::mat4& matrix) const
	{
		HZ_CORE_ASSERT(m_UniformLocations.find(name) != m_UniformLocations.end(), "");
		const GLint location = m_UniformLocations.at(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::SetUniform(const std::string& name, const glm::mat3& matrix) const
	{
		HZ_CORE_ASSERT(m_UniformLocations.find(name) != m_UniformLocations.end(), "");
		const GLint location = m_UniformLocations.at(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::Reflect(std::vector<uint32_t>& shaderData)
	{
		const spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		HZ_CORE_TRACE("OpenGLShader::Reflect - {0}", m_AssetPath);
		HZ_CORE_TRACE("		{0} uniform buffers", resources.uniform_buffers.size());
		HZ_CORE_TRACE("		{0} resources", resources.sampled_images.size());

		//TODO, DO WE NEED THIS?
		glUseProgram(m_RenderID);
		
		HZ_CORE_TRACE("Uniform buffers:");
		for (auto && resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			int memberCount = bufferType.member_types.size();
			uint32_t bindingPoint = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);

			if(s_UniformBuffers.find(bindingPoint) == s_UniformBuffers.end())
			{
				ShaderUniformBuffer& buffer = s_UniformBuffers[bindingPoint];
				buffer.Name = resource.name;
				buffer.BindingPoint = bindingPoint;
				buffer.Size = bufferSize;

				glCreateBuffers(1, &buffer.RenderID);
				glBindBuffer(GL_UNIFORM_BUFFER, buffer.RenderID);
				glBufferData(GL_UNIFORM_BUFFER, buffer.Size, nullptr, GL_DYNAMIC_DRAW);
				glBindBufferBase(GL_UNIFORM_BUFFER, buffer.BindingPoint, buffer.RenderID);
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				HZ_CORE_TRACE("Created Uniform Buffer at binding point {0} with name '{1}', size is {2} bytes", buffer.BindingPoint, buffer.Name, buffer.Size);
			}
			else
			{
				// Validation
				ShaderUniformBuffer& buffer = s_UniformBuffers[bindingPoint];
				HZ_CORE_ASSERT(buffer.Name == resource.name, "");
				// Resize
				if(bufferSize > buffer.Size)
				{
					buffer.Size = bufferSize;
					glDeleteBuffers(1, &buffer.RenderID);
					glCreateBuffers(1, &buffer.RenderID);
					glBindBuffer(GL_UNIFORM_BUFFER, buffer.RenderID);
					glBufferData(GL_UNIFORM_BUFFER, buffer.Size, nullptr, GL_DYNAMIC_DRAW);
					glBindBufferBase(GL_UNIFORM_BUFFER, buffer.BindingPoint, buffer.RenderID);
					
					HZ_CORE_TRACE("Resize Uniform Buffer at binding point {0} with name '{1}', size is {2} bytes", buffer.BindingPoint, buffer.Name, buffer.Size);
				}
			}
		}

		int32_t sampler = 0;
		for(auto& resource: resources.sampled_images)
		{
			const auto& name = resource.name;
			auto& type = compiler.get_type(resource.base_type_id);
			const auto binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t dimension = type.image.dim;

			const GLint location = glGetUniformLocation(m_RenderID, name.c_str());
			m_Resources[name] = ShaderResourceDeclaration(name, binding, 1);
			glUniform1i(location, binding);
		}
	}


	size_t OpenGLShader::GetHash() const
	{
		return std::hash<std::string>{}(m_AssetPath);
	}

	const std::unordered_map<std::string, ShaderBuffer>& OpenGLShader::GetShaderBuffers() const
	{
		return m_Buffers;
	}

	const std::unordered_map<std::string, ShaderResourceDeclaration>& OpenGLShader::GetResources() const
	{
		return m_Resources;
	}


	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RenderID);
	}

	void OpenGLShader::AddShaderReloadedCallback(const ShaderReloadedCallback& callback)
	{
		m_ShaderReloadedCallbacks.push_back(callback);
	}
	
	void OpenGLShader::UploadUniformInt(uint32_t location, int value) const
	{
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(uint32_t location, int* value, uint32_t count) const
	{
		glUniform1iv(location, count, value);
	}

	void OpenGLShader::UploadUniformFloat(uint32_t location, float value) const
	{
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(uint32_t location, const glm::vec2& value) const
	{
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(uint32_t location, const glm::vec3& value) const
	{
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(uint32_t location, const glm::vec4& value) const
	{
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat4(uint32_t location, const glm::mat4& matrix) const
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat3(uint32_t location, const glm::mat3& matrix) const
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value) const
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* value, uint32_t count) const
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform1iv(location, count, value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value) const
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value) const
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value) const
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values) const
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const
	{
		GLint location = GetUniformLocation(name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}


	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const
	{
		// Usually, the shader is pre-parsed to get the uniforms and locations
		// Uniforms groups to TWO kind, One kind is set by Renderer, like light direction, viewProjection etc.
		// the other kind is set by Material, like color
		GLint location = GetUniformLocation(name.c_str());
		// If we use a row-major matrix, we should transport this matrix, IE pass 3rd argument to GL_TRUE 
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}


	std::string OpenGLShader::ReadShaderFromFile(const std::string& filepath) const
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
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
			HZ_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specification");

			//normal shader start
			//size_t rawShaderStart = source.find_first_of("\r\n", eolOfToken);
			size_t rawShaderStart = eolOfToken + 1;
			HZ_CORE_ASSERT(rawShaderStart != std::string::npos, "Syntax error");
			typeTokenPos = source.find(typeToken, rawShaderStart);
			size_t count = rawShaderStart;
			const std::string& rawSource = source.substr(rawShaderStart, typeTokenPos - count);
			shaderSources[Utils::ShaderTypeFromString(type)] = rawSource;
		}
		return shaderSources;
	}

	// shaderSource is writen in Vulkan glsl, not Opengl glsl
	void OpenGLShader::CompileOrGetVulkanBinaries(std::unordered_map<uint32_t, std::vector<uint32_t>>& outputBinary, bool forceCompile)
	{
		std::filesystem::path cacheDirection = Utils::GetCacheDirectory();
		for (auto && [stage, source] : m_ShaderSource)
		{
			auto extension = Utils::GLShaderStageCachedVulkanFileExtension(stage);
			std::filesystem::path shaderFilePath = m_AssetPath;
			if(!forceCompile)
			{
				std::filesystem::path p = m_AssetPath;
				auto path = cacheDirection / (p.filename().string() + extension);
				std::string cachedFilePath = path.string();
				std::ifstream in(cachedFilePath, std::ios::in | std::ios::binary);
				if(in.is_open())
				{
					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto& data = outputBinary[stage];
					data.resize(size / sizeof(uint32_t));
					in.read((char*)data.data(), size);
					in.close();
				}
			}
			if((outputBinary[stage].size() == 0) || forceCompile)
			{
				// compile shader
				shaderc::Compiler compiler;
				shaderc::CompileOptions options;
				options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
				const bool optimize = true;
				if(optimize)
					options.SetOptimizationLevel(shaderc_optimization_level_performance);
				
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_AssetPath.c_str(), options);
				if(module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					HZ_CORE_ERROR(module.GetErrorMessage());
					HZ_CORE_ASSERT(false, "");
				}
				
				outputBinary[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				// cache compiled shader
				std::filesystem::path p = m_AssetPath;
				auto path = cacheDirection / (p.filename().string() + extension);
				std::string cachedFilePath = path.string();
				std::ofstream out(cachedFilePath, std::ios::out | std::ios::binary);
				if(out.is_open())
				{
					auto& data = outputBinary[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries(const std::unordered_map<uint32_t, std::vector<uint32_t>>& vulkanBinaries, bool forceCompile)
	{
		if(m_RenderID)
			glDeleteProgram(m_RenderID);

		GLuint program = glCreateProgram();
		m_RenderID = program;

		std::vector<GLuint> shaderRenderIDs;
		shaderRenderIDs.reserve(vulkanBinaries.size());

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();
		m_ConstantBufferOffset = 0;
		std::vector<std::vector<uint32_t>> shaderData;	
		for (auto && [stage, spirv] : vulkanBinaries)
		{
			spirv_cross::CompilerGLSL glslCompiler(spirv);
			ParseConstantBuffers(glslCompiler);
			
			std::vector<uint32_t>& shaderStageData = shaderData.emplace_back();
			std::filesystem::path shaderFilePath = m_AssetPath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));
			if(!forceCompile)
			{
				std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
				if(in.is_open())
				{
					// file exist
					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto& data = shaderStageData;
					data.resize(size / sizeof(uint32_t));
					in.read((char*)data.data(), size);
				}
			}
			if(forceCompile || shaderStageData.size() == 0)
			{
				// compile vulkan glsl to opengl glsl
				shaderc::Compiler compiler;
				shaderc::CompileOptions options;
				options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
			
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				std::string generatedSource = glslCompiler.compile();

				printf("================================================\n");
				printf("%s Shader:\n %s\n", Utils::GLShaderTypeToString(stage), generatedSource.c_str());
				printf("=================================================\n");

				// input_file_name parameter is a tag for debug
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(generatedSource, Utils::GLShaderStageToShaderC(stage), m_AssetPath.c_str(), options);
				if(module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					HZ_CORE_ERROR(module.GetErrorMessage());
					HZ_CORE_ASSERT(false, "");
				}

				shaderStageData = std::vector<uint32_t>(module.cbegin(), module.cend());
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if(out.is_open())
				{
					auto& data = shaderStageData;
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}

			GLuint shaderID = glCreateShader(stage);
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderStageData.data(), shaderStageData.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);

			shaderRenderIDs.emplace_back(shaderID);
		}

		glLinkProgram(program);
		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if(isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			HZ_CORE_ERROR("Shader linking failed: ({0}): \n {1}", m_AssetPath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderRenderIDs)
			{
				glDeleteShader(id);
			}
		}

		// Always detach shaders after a successful link
		for (auto id : shaderRenderIDs)
		{
			glDetachShader(program, id);
		}

		//TODO
		for(auto& [bufferName, buffer] : m_Buffers)
		{
			for(auto& [name, uniform] : buffer.Uniforms)
			{
				GLint location = glGetUniformLocation(m_RenderID, name.c_str());
				if(location == -1)
					HZ_CORE_WARN("{0}: could not find uniform location {0}", name);

				m_UniformLocations[name] = location;
			}
		}

		for(auto& shaderStageData : shaderData)
		{
			Reflect(shaderStageData);
		}
	}

	void OpenGLShader::ParseConstantBuffers(const spirv_cross::CompilerGLSL& compiler)
	{
		auto res = compiler.get_shader_resources();
		for(auto& resource : res.push_constant_buffers)
		{
			const auto& bufferName = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			auto bufferSize = compiler.get_declared_struct_size(bufferType);

			if(bufferName.empty() || bufferName == "u_Renderer")
			{
				m_ConstantBufferOffset += bufferSize;
				continue;
			}
			auto location = compiler.get_decoration(resource.id, spv::DecorationLocation);
			int memberCount = bufferType.member_types.size();
			ShaderBuffer& buffer = m_Buffers[bufferName];
			buffer.Name = bufferName;
			buffer.Size = bufferSize - m_ConstantBufferOffset;
			for(int i=0; i< memberCount; ++i)
			{
				auto type = compiler.get_type(bufferType.member_types[i]);
				const auto& memberName = compiler.get_member_name(bufferType.self, i);
				auto size = compiler.get_declared_struct_member_size(bufferType, i);
				auto offset = compiler.type_struct_member_offset(bufferType, i) - m_ConstantBufferOffset;

				std::string uniformName = bufferName + "." + memberName;
				buffer.Uniforms[uniformName] = ShaderUniform(uniformName, Utils::SPIRTypeToShaderUniformType(type), size, offset);
			}

			m_ConstantBufferOffset += bufferSize;
		}
	}

	int32_t OpenGLShader::GetUniformLocation(const std::string& name) const
	{
		int32_t result = glGetUniformLocation(m_RenderID, name.c_str());
		if(result == -1)
			HZ_CORE_WARN("Could not find uniform '{0}' in shader '{1}' ", name, m_AssetPath);
		return result;
	}
}

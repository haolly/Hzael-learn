#include "hazelPCH.h"
#include "Hazel/Renderer/Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer.h"

namespace Hazel
{
	std::vector<Ref<Shader>> Shader::s_AllShaders;

	ShaderUniform::ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset)
		: m_Name(name), m_Type(type), m_Size(size), m_Offset(offset)
	{
	}

	const std::string& ShaderUniform::UniformTypeToString(ShaderUniformType type)
	{
		switch (type)
		{
			case ShaderUniformType::Bool:
				return "Boolean";
			
			case ShaderUniformType::Int:
				return "Int";
			
			case ShaderUniformType::Float:
				return "Float";
		}
		return "None";
	}

	Ref<Shader> Shader::CreateFromString(const std::string& source)
	{
		Ref<Shader> result = nullptr;
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
			{
				HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				result = OpenGLShader::CreateFromString(source);
				break;
			}
		}
		if(!result)
		{
			HZ_CORE_ASSERT(false, "RendererAPI is not set");
			return nullptr;
		}
		s_AllShaders.push_back(result);
		return result;
	}
	
	Ref<Shader> Shader::Create(const std::string& filepath, bool forceCompile)
	{
		Ref<Shader> result = nullptr;
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
			{
				HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
				return nullptr;
			}
			case RendererAPI::API::OpenGL:
			{
				result = Ref<OpenGLShader>::Create(filepath, forceCompile);
				break;
			}
		}
		if(!result)
		{
			HZ_CORE_ASSERT(false, "RendererAPI is not set");
			return nullptr;
		}
		s_AllShaders.push_back(result);
		return result;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		HZ_CORE_ASSERT(!Exist(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		HZ_CORE_ASSERT(Exist(name), "Shader doesn't exists!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exist(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}

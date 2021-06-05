#include "hazelPCH.h"
#include "OpenGLMaterial.h"

#include "OpenGLImage2D.h"
#include "OpenGLShader.h"
#include "Hazel/Renderer/Renderer.h"

namespace Hazel
{
	OpenGLMaterial::OpenGLMaterial(const Ref<Shader>& shader, const std::string& name)
		: m_Shader(shader), m_Name(name)
	{
		AllocateStorage();

		m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
		m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;
	}

	OpenGLMaterial::~OpenGLMaterial()
	{
	}

	void OpenGLMaterial::Invalidate()
	{
	}

	void OpenGLMaterial::Set(const std::string& name, float value)
	{
		Set<float>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, int value)
	{
		Set<int>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, bool value)
	{
		Set<uint32_t>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::vec2 value)
	{
		Set<glm::vec2>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::vec3 value)
	{
		Set<glm::vec3>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::vec4 value)
	{
		Set<glm::vec4>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::mat3 value)
	{
		Set<glm::mat3>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const glm::mat4 value)
	{
		Set<glm::mat4>(name, value);
	}

	void OpenGLMaterial::Set(const std::string& name, const Ref<Texture2D>& texture)
	{
		auto decl = FindResourceDeclaration(name);
		if(!decl)
		{
			HZ_CORE_WARN("Cannot find material property: {0}", name);
			return;
		}
		uint32_t slot = decl->GetRegister();
		m_Texture2Ds[slot] = texture;
	}

	void OpenGLMaterial::Set(const std::string& name, const Ref<Image2D>& image)
	{
		auto decl = FindResourceDeclaration(name);
		if(!decl)
		{
			HZ_CORE_WARN("Cannot find material property: {0}", name);
			return;
		}
		uint32_t slot = decl->GetRegister();
		m_Images[slot] = image;
	}

	float& OpenGLMaterial::GetFloat(const std::string& name)
	{
		return Get<float>(name);
	}

	int32_t& OpenGLMaterial::GetInt(const std::string& name)
	{
		return Get<int>(name);
	}

	uint32_t& OpenGLMaterial::GetUInt(const std::string& name)
	{
		return Get<uint32_t>(name);
	}

	bool& OpenGLMaterial::GetBool(const std::string& name)
	{
		return Get<bool>(name);
	}

	glm::vec2& OpenGLMaterial::GetVector2(const std::string& name)
	{
		return Get<glm::vec2>(name);
	}

	glm::vec3& OpenGLMaterial::GetVector3(const std::string& name)
	{
		return Get<glm::vec3>(name);
	}

	glm::vec4& OpenGLMaterial::GetVector4(const std::string& name)
	{
		return Get<glm::vec4>(name);
	}

	glm::mat3& OpenGLMaterial::GetMatrix3(const std::string& name)
	{
		return Get<glm::mat3>(name);
	}

	glm::mat4& OpenGLMaterial::GetMatrix4(const std::string& name)
	{
		return Get<glm::mat4>(name);
	}

	Ref<Texture2D> OpenGLMaterial::GetTexture2D(const std::string& name)
	{
		auto decl = FindResourceDeclaration(name);
		HZ_CORE_ASSERT(decl, "Could not find uniform with name 'xxxxx' ")
		uint32_t slot = decl->GetRegister();
		HZ_CORE_ASSERT(slot < m_Texture2Ds.size(), "Texture alot is invalid");
		return m_Texture2Ds[slot];
	}

	void OpenGLMaterial::UpdateForRendering()
	{
		Ref<OpenGLShader> shader = m_Shader.As<OpenGLShader>();
		Renderer::Submit([shader]()
		{
			glUseProgram(shader->GetRendererID());
		});

		const auto& shaderBuffers = m_Shader->GetShaderBuffers();
		HZ_CORE_ASSERT(shaderBuffers.size() <= 1, "Currently only support ONE material buffer!");
		if(shaderBuffers.size() > 0)
		{
			const ShaderBuffer& buffer = shaderBuffers.begin()->second;
			for(auto& [name, uniform] : buffer.Uniforms)
			{
				switch (uniform.GetType())
				{
					case ShaderUniformType::Bool:
					case ShaderUniformType::UInt:
						{
							const uint32_t value = m_UniformStorageBuffer.Read<uint32_t>(uniform.GetOffset());
							shader->SetUniform(name, value);
							break;
						}
					case ShaderUniformType::Int:
						{
							const int value = m_UniformStorageBuffer.Read<int>(uniform.GetOffset());
							shader->SetUniform(name, value);
							break;
						}
					case ShaderUniformType::Float:
						{
							const float value = m_UniformStorageBuffer.Read<float>(uniform.GetOffset());
							shader->SetUniform(name, value);
							break;
						}
					case ShaderUniformType::Vec2:
						{
							const glm::vec2& value = m_UniformStorageBuffer.Read<glm::vec2>(uniform.GetOffset());
							shader->SetUniform(name, value);
							break;
						}
					case ShaderUniformType::Vec3:
						{
							const glm::vec3& value = m_UniformStorageBuffer.Read<glm::vec3>(uniform.GetOffset());
							shader->SetUniform(name, value);
							break;
						}
					case ShaderUniformType::Vec4:
						{
							const glm::vec4& value = m_UniformStorageBuffer.Read<glm::vec4>(uniform.GetOffset());
							shader->SetUniform(name, value);
							break;
						}
					case ShaderUniformType::Mat3:
						{
							const glm::mat3& value = m_UniformStorageBuffer.Read<glm::mat3>(uniform.GetOffset());
							shader->SetUniform(name, value);
							break;
						}
					case ShaderUniformType::Mat4:
						{
							const glm::mat4& value = m_UniformStorageBuffer.Read<glm::mat4>(uniform.GetOffset());
							shader->SetUniform(name, value);
							break;
						}
				default:
						{
							HZ_CORE_ASSERT(false, "");
							break;
						};
				}
			}
		}

		for(auto& [slot, texture] : m_Texture2Ds)
		{
			if(texture)
			{
				uint32_t textureSlot = slot;
				Ref<Image2D> image = texture->GetImage();
				Ref<OpenGLImage2D> glImage = image.As<OpenGLImage2D>();
				Renderer::Submit([textureSlot, glImage]()
				{
					glBindSampler(textureSlot, glImage->GetSamplerRendererID());
					glBindTextureUnit(textureSlot, glImage->GetRendererID());
				});
			}
		}

		for(auto& [slot, image] : m_Images)
		{
			if(image)
			{
				uint32_t textureSlot = slot;
				Ref<OpenGLImage2D> glImage = image.As<OpenGLImage2D>();
				Renderer::Submit([textureSlot, glImage]()
				{
					glBindSampler(textureSlot, glImage->GetSamplerRendererID());
					glBindTextureUnit(textureSlot, glImage->GetRendererID());
				});
			}
		}
	}

	void OpenGLMaterial::AllocateStorage()
	{
		const auto& shaderBuffers = m_Shader->GetShaderBuffers();
		if(shaderBuffers.size() > 0)
		{
			uint32_t size = 0;
			for(auto& [name, shaderBuffer] : shaderBuffers)
			{
				size += shaderBuffer.Size;
			}

			m_UniformStorageBuffer.Allocate(size);
			m_UniformStorageBuffer.ZeroInitialize();
		}
	}

	void OpenGLMaterial::OnShaderReloaded()
	{
	}

	const ShaderUniform* OpenGLMaterial::FindUniformDeclaration(const std::string& name) const
	{
		const auto& shaderBuffers = m_Shader->GetShaderBuffers();
		HZ_CORE_ASSERT(shaderBuffers.size() <= 1, "Only support ONE material buffer");
		if(shaderBuffers.size() > 0)
		{
			const ShaderBuffer& buffer = shaderBuffers.begin()->second;
			if(buffer.Uniforms.find(name) == buffer.Uniforms.end())
				return nullptr;

			return &buffer.Uniforms.at(name);
		}
		return nullptr;
	}

	const ShaderResourceDeclaration* OpenGLMaterial::FindResourceDeclaration(const std::string& name) const
	{
		auto& resources = m_Shader->GetResources();
		for(const auto& [name, resource] : resources)
		{
			if(resource.GetName() == name)
				return &resource;
		}
		return nullptr;
	}
}

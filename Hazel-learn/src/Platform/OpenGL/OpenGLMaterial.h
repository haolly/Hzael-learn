#pragma once
#include <map>

#include "Hazel/Core/Buffer.h"
#include "Hazel/Renderer/Material.h"

namespace Hazel
{
	class OpenGLMaterial : public Material
	{
	public:
		explicit OpenGLMaterial(const Ref<Shader>& shader, const std::string& name = "");
		~OpenGLMaterial() override;
		void Invalidate() override;
		void Set(const std::string& name, float value) override;
		void Set(const std::string& name, int value) override;
		void Set(const std::string& name, bool value) override;
		void Set(const std::string& name, const glm::vec2 value) override;
		void Set(const std::string& name, const glm::vec3 value) override;
		void Set(const std::string& name, const glm::vec4 value) override;
		void Set(const std::string& name, const glm::mat3 value) override;
		void Set(const std::string& name, const glm::mat4 value) override;
		void Set(const std::string& name, const Ref<Texture2D>& texture) override;
		float& GetFloat(const std::string& name) override;
		int32_t& GetInt(const std::string& name) override;
		uint32_t& GetUInt(const std::string& name) override;
		bool& GetBool(const std::string& name) override;
		glm::vec2& GetVector2(const std::string& name) override;
		glm::vec3& GetVector3(const std::string& name) override;
		glm::vec4& GetVector4(const std::string& name) override;
		glm::mat3& GetMatrix3(const std::string& name) override;
		glm::mat4& GetMatrix4(const std::string& name) override;
		Ref<Texture2D> GetTexture2D(const std::string& name) override;

		template<typename T>
		void Set(const std::string& name, const T& value)
		{
			const auto decl = FindUniformDeclaration(name);
			HZ_CORE_ASSERT(decl, "Could not find uniform");
			if(!decl)
				return;

			auto& buffer = m_UniformStorageBuffer;
			buffer.Write((std::byte*)&value, decl->GetSize(), decl->GetOffset());
		}

		template<typename T>
		T& Get(const std::string& name)
		{
			auto decl = FindUniformDeclaration(name);
			HZ_CORE_ASSERT(decl, "Could not find uniform");
			auto& buffer = m_UniformStorageBuffer;
			return buffer.Read<T>(decl->GetOffset());
		}
		
		void UpdateForRendering();
	private:
		void AllocateStorage();
		void OnShaderReloaded();

		const ShaderUniform* FindUniformDeclaration(const std::string& name) const;
		const ShaderResourceDeclaration* FindResourceDeclaration(const std::string& name) const;
	private:
		Ref<Shader> m_Shader;
		std::string m_Name;
		uint32_t m_MaterialFlags = 0;

		// store all constant data(uniform ?)  for this material with respect to m_Shader
		Buffer m_UniformStorageBuffer;
		std::map<uint32_t, Ref<Texture2D>> m_Texture2Ds;
	};
}

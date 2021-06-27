#pragma once
#include <string>
#include <unordered_map>

#include "ShaderUniform.h"
#include "glm/fwd.hpp"

namespace Hazel
{
	enum class ShaderUniformType
	{
		None = 0,
		Bool,
		Int,
		UInt,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Mat3,
		Mat4
	};

	class ShaderUniform
	{
	public:
		ShaderUniform() = default;
		ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset);

		const std::string& GetName() const
		{
			return m_Name;
		}
		ShaderUniformType GetType() const { return m_Type;}
		uint32_t GetSize() const { return m_Size;}
		uint32_t GetOffset() const { return m_Offset;}

		static const std::string& UniformTypeToString(ShaderUniformType type);
	private:
		std::string m_Name;
		ShaderUniformType m_Type = ShaderUniformType::None;
		uint32_t m_Size = 0;
		uint32_t m_Offset = 0;
	};

	struct ShaderUniformBuffer
	{
		std::string Name;
		uint32_t Index;
		uint32_t BindingPoint;
		uint32_t Size;
		uint32_t RenderID;
		std::vector<ShaderUniform> Uniforms;
	};

	// const buffer
	struct ShaderBuffer
	{
		std::string Name;
		uint32_t Size = 0;
		std::unordered_map<std::string, ShaderUniform> Uniforms;
	};
	
	class Shader : public RefCounted
	{
	public:
		using ShaderReloadedCallback = std::function<void()>;

		virtual void Reload(bool forceCompile = false) = 0;
		virtual size_t GetHash() const = 0;

		virtual void Bind() const = 0;
		
		virtual const std::string& GetName() const = 0;
		static Ref<Shader> Create(const std::string& filepath, bool forceCompile = false);
		static Ref<Shader> CreateFromString(const std::string& source);
		
		virtual const std::unordered_map<std::string, ShaderBuffer>& GetShaderBuffers() const = 0;
		virtual const std::unordered_map<std::string, ShaderResourceDeclaration>& GetResources() const = 0;

		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) = 0;

		static std::vector<Ref<Shader>> s_AllShaders;
	};

	//TODO, replace by Asset Manager
	class ShaderLibrary : public RefCounted
	{
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);
		Ref<Shader> Get(const std::string& name);
		bool Exist(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}

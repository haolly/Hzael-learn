#pragma once
#include "Shader.h"
#include "Texture.h"

namespace Hazel
{
	enum class MaterialFlag
	{
		None = BIT(0),
		DepthTest = BIT(1),
		Blend = BIT(2),
		TwoSided = BIT(3)
	};
	class Material : public RefCounted
	{
	public:
		static Ref<Material> Create(const Ref<Shader>& shader, const std::string& name = "");
		virtual ~Material(){}

		virtual void Invalidate() = 0;

		virtual void Set(const std::string& name, float value) = 0;
		virtual void Set(const std::string& name, int value) = 0;
		virtual void Set(const std::string& name, bool value) = 0;
		virtual void Set(const std::string& name, const glm::vec2 value) = 0;
		virtual void Set(const std::string& name, const glm::vec3 value) = 0;
		virtual void Set(const std::string& name, const glm::vec4 value) = 0;
		virtual void Set(const std::string& name, const glm::mat3 value) = 0;
		virtual void Set(const std::string& name, const glm::mat4 value) = 0;

		virtual void Set(const std::string& name, const Ref<Texture2D>& texture) = 0;
		virtual void Set(const std::string& name, const Ref<Image2D>& image) = 0;

		virtual float& GetFloat(const std::string& name) = 0;
		virtual int32_t& GetInt(const std::string& name) = 0;
		virtual uint32_t& GetUInt(const std::string& name) = 0;
		virtual bool& GetBool(const std::string& name) = 0;
		virtual glm::vec2& GetVector2(const std::string& name) = 0;
		virtual glm::vec3& GetVector3(const std::string& name) = 0;
		virtual glm::vec4& GetVector4(const std::string& name) = 0;
		virtual glm::mat3& GetMatrix3(const std::string& name) = 0;
		virtual glm::mat4& GetMatrix4(const std::string& name) = 0;

		virtual Ref<Texture2D> GetTexture2D(const std::string& name) = 0;
	};
}

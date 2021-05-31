#include "hazelPCH.h"
#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>


#include "Components.h"
#include "Entity.h"

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if(!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if(!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Hazel
{
	YAML::Emitter& operator <<(YAML::Emitter& out , const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator <<(YAML::Emitter& out , const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;	// Entity
		out << YAML::Key << "Entity" << YAML::Value << "12823589323333";

		if(entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";		
			out << YAML::BeginMap;	// TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;	// TagComponent
		}

		
		if(entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";		
			out << YAML::BeginMap;	// TransformComponent

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap;	// TransformComponent
		}

		if(entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";		
			out << YAML::BeginMap;	//CameraComponent 

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap;	// Camera
			
			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap;	//CameraComponent 
		}

		
		if(entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";		
			out << YAML::BeginMap;	//SpriteRendererComponent 

			auto& spriteComp = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteComp.Color;

			out << YAML::EndMap;	//SpriteRendererComponent 
		}


		out << YAML::EndMap;	// Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::Value << "Untitled";
		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = {entityID, m_Scene.Raw()};
			if(!entity)
				return;

			SerializeEntity(out, entity);
		});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		HZ_CORE_ASSERT(false, "Not implemented");
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if(!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		HZ_CORE_TRACE("Deserializing scene '{0}' ", sceneName);

		auto entities = data["Entities"];
		if(entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO, make uuid

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if(tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				HZ_CORE_TRACE("Deserialized entity with ID={0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntity(name);

				auto transformComp = entity["TransformComponent"];
				if(transformComp)
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComp["Translation"].as<glm::vec3>();
					tc.Rotation = transformComp["Rotation"].as<glm::vec3>();
					tc.Scale = transformComp["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if(cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteComponent = entity["SpriteRendererComponent"];
				if(spriteComponent)
				{
					auto& spriteComp = deserializedEntity.AddComponent<SpriteRendererComponent>();
					spriteComp.Color = spriteComponent["Color"].as<glm::vec4>();
				}
			}
		}
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		HZ_CORE_ASSERT(false, "Not implemented");
		return false;
	}
}

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "SceneCamera.h"
#include "glm/gtx/quaternion.hpp"
#include "Hazel/Core/UUID.h"

namespace Hazel
{

	struct IDComponent
	{
		UUID ID = 0;
	};

	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};
	
	struct TransformComponent
	{
		glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
		glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
		glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

		glm::vec3 Up = {0.0f, 1.0f, 0.0f};
		glm::vec3 Right = {1.0f, 0.0f, 0.0f};
		glm::vec3 Forward = {0.0f, 0.0f, -1.0f};	//todo

		TransformComponent() = default;
		TransformComponent(const glm::vec3& translation) : Translation(translation) {}


		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}

	};

	struct RelationshipComponent
	{
		UUID ParentHandle = 0;
		std::vector<UUID> Children;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;
		RelationshipComponent(UUID parent)
			: ParentHandle(parent)
		{
			
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{1.0f};

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		//TODO, MOVING TO SCENE
		bool Primary = true;
		bool FixedAspectRatio = false;
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		// CameraComponent(const glm::mat4& projection) : Camera(projection) {}
	};
	
}

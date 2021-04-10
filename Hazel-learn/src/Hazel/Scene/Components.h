#pragma once

#include <glm/glm.hpp>


#include "SceneCamera.h"
#include "Hazel/Renderer/Camera.h"

namespace Hazel
{
	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};
	
	struct TransformComponent
	{
		glm::mat4 Transform{1.0f};

		TransformComponent() = default;
		TransformComponent(const glm::mat4& transform) : Transform(transform) {}

		operator glm::mat4& () { return Transform;}
		operator const glm::mat4& () const { return Transform;}
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
		// CameraComponent(const glm::mat4& projection) : Camera(projection) {}
	};
	
}

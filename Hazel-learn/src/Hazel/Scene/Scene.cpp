#include "hazelPCH.h"
#include "Scene.h"
#include "Hazel/Scene/Components.h"
#include "Hazel/Renderer/Renderer2D.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = {m_Registry.create(), this};
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "UnNamedEntity" : name;
		return entity;
	}

	void Scene::OnUpdate(float ts)
	{
		Camera* mainCamera = nullptr;
		TransformComponent* transformComp = nullptr;
		auto view = m_Registry.view<CameraComponent, TransformComponent>();
		for (auto entity : view)
		{
			auto& [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);
			if(camera.Primary)
			{
				mainCamera = &camera.Camera;
				transformComp = &transform;
				break;
			}
		}

		if(mainCamera)
		{

			Renderer2D::BeginScene(*mainCamera, transformComp->Transform);
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawQuad(transform, sprite.Color);
			}
			Renderer2D::EndScene();
		}

		
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize non-fixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComp = view.get<CameraComponent>(entity);
			if(!cameraComp.FixedAspectRatio)
			{
				cameraComp.Camera.SetViewportSize(width, height);
			}
		}

	}
}

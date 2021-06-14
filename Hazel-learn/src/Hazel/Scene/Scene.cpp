#include "hazelPCH.h"
#include "Scene.h"
#include "Hazel/Scene/Components.h"
#include "Hazel/Renderer/Renderer2D.h"
#include "Hazel/Scene/Entity.h"
#include "Hazel/Script/ScriptEngine.h"

namespace Hazel
{

	static void OnScriptComponentConstruct(entt::registry& registry, entt::entity entity)
	{
		//todo
		ScriptEngine::InitScriptEntity()
	}
	
	static void OnScriptComponentDestroy(entt::registry& registry, entt::entity entity)
	{
		//todo
	}

	/// <summary>
	/// Note, 这些模板特例化必须在使用任何实例之前，不然就会报错 https://docs.microsoft.com/en-us/cpp/error-messages/compiler-errors-2/compiler-error-c2908?view=msvc-160
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="entity"></param>
	/// <param name="component"></param>
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	Scene::Scene()
	{
		m_Registry.on_construct<ScriptComponent>().connect<&OnScriptComponentConstruct>();
		m_Registry.on_destroy<ScriptComponent>().connect<&OnScriptComponentDestroy>();
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

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateEditor(float ts, EditorCamera& camera)
	{
		// 本质上就是 shader 中的viewProjection 不同
		Renderer2D::BeginScene(camera);
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}
		Renderer2D::EndScene();

	}

	void Scene::OnUpdateRuntime(float ts)
	{
		{
			// TODO move to Scene::OnScenePlay, and call scriptComponent.Instance->OnDestroy on Scene::OnStopPlay
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& scriptComponent)
			{
				if(!scriptComponent.Instance)
				{
					scriptComponent.Instance = scriptComponent.InstantiateScript();
					scriptComponent.Instance->m_Entity = Entity{entity, this};
					scriptComponent.Instance->OnCreate();
				}
				scriptComponent.Instance->OnUpdate(ts);
			});
		}

		Camera* mainCamera = nullptr;
		TransformComponent* mainCameraTrans = nullptr;
		auto view = m_Registry.view<CameraComponent, TransformComponent>();
		for (auto entity : view)
		{
			auto [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);
			if(camera.Primary)
			{
				mainCamera = &camera.Camera;
				mainCameraTrans = &transform;
				break;
			}
		}

		if(mainCamera)
		{

			Renderer2D::BeginScene(*mainCamera, mainCameraTrans->GetTransform());
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
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

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto && entity : view)
		{
			const auto& cameraComp = view.get<CameraComponent>(entity);
			if(cameraComp.Primary)
				return Entity{entity, this};
		}
		return {};
	}

}

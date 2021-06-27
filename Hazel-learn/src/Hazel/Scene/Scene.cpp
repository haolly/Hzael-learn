#include "hazelPCH.h"
#include "Scene.h"
#include "Hazel/Scene/Components.h"
#include "Hazel/Renderer/Renderer2D.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel
{

	/// <summary>
	/// Note,  https://docs.microsoft.com/en-us/cpp/error-messages/compiler-errors-2/compiler-error-c2908?view=msvc-160
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
		if(m_ViewportWidth >0 && m_ViewportHeight > 0)
		{
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		}
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

	Scene::Scene(const std::string& debugName, bool isEditorScene)
		: m_DebugName(debugName), m_IsEditorScene(isEditorScene)
	{
		// TODO, connect scriptcomponent
		Init();
	}

	void Scene::Init()
	{
	}

	void Scene::OnUpdate(float ts)
	{
	}

	void Scene::OnRenderRuntime(Ref<SceneRenderer> renderer, float ts)
	{
	}

	void Scene::OnRenderEditor(Ref<SceneRenderer> renderer, float ts, const EditorCamera& editorCamera)
	{
	}

	void Scene::OnEvent(Event& e)
	{
	}

	Scene::~Scene()
	{
		m_Registry.clear();
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

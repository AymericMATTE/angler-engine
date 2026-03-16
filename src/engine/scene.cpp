#include "pch.h"
#include "scene.h"
#include "application.h"

namespace angler {
	void Scene::Awake() {
		OnAwake();
	}

	void Scene::Start() {
		if (m_isEnabled == false)
			return;

		for (GameObject* gameObject : m_gameObjects) {
			if (gameObject == nullptr)
				continue;

			gameObject->Start();
		}

		if(m_started == false) {
			OnStart();
			m_started = true;
		}
	}

	void Scene::FixedUpdate() {
		if (m_isEnabled == false)
			return;

		OnFixedUpdate();
		for (GameObject* gameObject : m_gameObjects) {
			if (gameObject == nullptr)
				continue;

			gameObject->FixedUpdate();
		}
	}

	void Scene::PreRender() {
		if (m_isEnabled == false)
			return;

		OnPreRender();
		for (GameObject* gameObject : m_gameObjects) {
			if (gameObject == nullptr)
				continue;

			gameObject->PreRender();
		}
	}

	void Scene::Render3D() {
		if (m_isEnabled == false)
			return;

		OnRender3D();
		for (GameObject* gameObject : m_gameObjects) {
			if (gameObject == nullptr)
				continue;

			gameObject->Render3D();
		}
	}

	void Scene::RenderUI()
	{
		if (m_isEnabled == false)
			return;

		OnRenderUI();
		for (GameObject* gameObject : m_gameObjects) {
			if (gameObject == nullptr)
				continue;

			gameObject->RenderUI();
		}
	}

	void Scene::Update() {
		if (m_isEnabled == false)
			return;

		OnUpdate();
		for (GameObject* gameObject : m_gameObjects) {
			if (gameObject == nullptr)
				continue;

			gameObject->Update();
		}
	}

	void Scene::Destroy() {
		Application::get().addToDestroy(this);
		if (m_isEnabled == false)
			return;

		OnDestroy();
		for (GameObject* gameObject : m_gameObjects) {
			if (gameObject == nullptr)
				continue;

			gameObject->Destroy();
		}
	}

	void  Scene::Enabled() {
		m_isEnabled = true;
	}
	void  Scene::Disabled() {
		m_isEnabled = false;
	}

	bool  Scene::isEnabled() {
		return m_isEnabled;
	}

	GameObject* Scene::CreateGameObject() {
		GameObject* newGameObject = new GameObject();
		newGameObject->m_sceneId = m_sceneId;

		if(m_freeId.empty()) {
			newGameObject->m_id = m_gameObjects.size();
			m_gameObjects.push_back(newGameObject);
		}
		else {
			newGameObject->m_id = m_freeId.front();
			m_gameObjects[newGameObject->m_id] = newGameObject;
			m_freeId.pop();
		}

		return newGameObject;
	}

	void Scene::SetMainCamera(Camera3DComponent* _camera)
	{
		if(m_mainCamera) m_mainCamera->m_isMain = false;
		m_mainCamera = _camera;		
		m_mainCamera->m_isMain = true;
	}

	std::vector<GameObject*>& Scene::getGameObjects() {
		return m_gameObjects;
	}

	void Scene::RemoveFromList(GameObject* _gameObject) {
		m_gameObjects[_gameObject->m_id] = nullptr;
		m_freeId.push(_gameObject->m_id);
	}
}
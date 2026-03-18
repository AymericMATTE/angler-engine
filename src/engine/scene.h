#pragma once
#include <vector>
#include <queue>
#include "ec/game-object.h"
#include "ec/components/camera-3d-component.h"

namespace angler
{
	class Scene
	{
	public :
		void Start();
		void Update();
		void FixedUpdate();
		void PreRender();
		void Render3D();
		void RenderUI();
		void Destroy();

		void Enabled();
		void Disabled();

		bool isEnabled();

		virtual void OnStart() {}
		virtual void OnUpdate() {}
		virtual void OnFixedUpdate() {}
		virtual void OnRender3D() {}
		virtual void OnRenderUI() {}
		virtual void OnPreRender() {}
		virtual void OnDestroy() {}

		GameObject* CreateGameObject();
		void SetMainCamera(Camera3DComponent* _camera);
		Camera3DComponent* GetMainCamera();

		std::vector<GameObject*>& getGameObjects();

	protected:
		Scene() = default;

	private:
		void RemoveFromList(GameObject* _gameObject);

		Camera3DComponent* m_mainCamera;

		std::vector<GameObject*> m_gameObjects = {};
		std::queue<unsigned int> m_freeId = {};
		int m_sceneId = 0;

		bool m_isEnabled = true;
		bool m_started = false;

		friend class SceneManager;
		friend class Application;
	};
}
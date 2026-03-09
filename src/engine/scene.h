#pragma once
#include <vector>
#include <queue>
#include "ec/gameObject.h"

namespace angler
{
	class Scene
	{
	public :
		void Start();
		void Update();
		void Render();
		void Destroy();

		void Enabled();
		void Disabled();

		bool isEnabled();

		virtual void OnStart() {}
		virtual void OnUpdate() {}
		virtual void OnRender() {}
		virtual void OnDestroy() {}

		GameObject* CreateGameObject();

		std::vector<GameObject*>& getGameObjects();

	private:
		Scene() = default;
		void RemoveFromList(GameObject* _gameObject);

		std::vector<GameObject*> m_gameObjects = {};
		std::queue<unsigned int> m_freeId = {};
		unsigned int m_sceneId = 0;

		bool m_isEnabled = false;

		friend class SceneManager;
		friend class Application;
	};
}
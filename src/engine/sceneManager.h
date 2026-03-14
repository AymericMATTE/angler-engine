#pragma once
#include <vector>
#include <queue>
#include "scene.h"

namespace angler
{
	class SceneManager
	{
	public :
		template<class T>
		T* CreateScene();

		void ChangeScene(Scene* _scene);

	private :
		SceneManager();
		void RemoveFromList(Scene* _scene);

		Scene* getScene(int _index);
		Scene* getGlobalScene();

		std::vector<Scene*> m_scenes = {};
		std::queue<unsigned int> m_scenefreeId = {};
		unsigned int m_activeScene = 0;

		Scene m_globalScene;

		friend class Application;
	};
}

#include "sceneManager.inl"
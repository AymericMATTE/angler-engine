#pragma once
#include <vector>
#include <queue>
#include "scene.h"

namespace angler
{
	class SceneManager
	{
	public :
		Scene* getScene(unsigned int _index);

		template<class T>
		T* CreateScene();

		void ChangeScene(Scene* _scene);

	private :
		SceneManager() = default;
		void RemoveFromList(Scene* _scene);

		std::vector<Scene*> m_scenes = {};
		std::queue<unsigned int> m_freeId = {};
		unsigned int m_activeScene = 0;

		friend class Application;
	};
}

#include "sceneManager.inl"
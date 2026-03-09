#pragma once
#include <unordered_map>

namespace angler
{
	struct Component;

	class GameObject
	{
	public :
		template<class T>
		T* addComponent();

		template<class T>
		T* getComponent();

		template<class T>
		bool hasComponent();

		template<class T>
		void removeComponent();

		void Enabled();
		void Disabled();

		bool isEnabled();

		void Start();
		void Update();
		void Render();

		void Destroy();

		~GameObject();

	private :
		GameObject() = default;

		unsigned int m_id = 0;
		unsigned int m_sceneId = 0;
		std::unordered_map<unsigned long long, Component*> m_components = {};

		bool m_toDestroy = false;
		bool m_isEnabled = true;

		GameObject* m_parent = nullptr;
		std::vector<GameObject*> m_childrens = {};

		friend class Scene;
		friend class Application;
	};
}

#include "gameObject.inl"


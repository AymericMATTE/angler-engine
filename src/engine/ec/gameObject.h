#pragma once
#include <unordered_map>

namespace angler
{
	struct Component;

	class GameObject
	{
	public :

		GameObject* getParent();
		void setParent(GameObject* _parent);

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
		void FixedUpdate();

		void Collide();

		void PreRender();
		void Render();

		void Destroy();

		~GameObject();

	private :
		GameObject() = default;
		void removeChild(GameObject* _child);
		void addChild(GameObject* _child);

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


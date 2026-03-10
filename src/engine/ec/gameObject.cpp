#include "gameObject.h"
#include "application.h"

namespace angler {
	GameObject* GameObject::getParent() {
		return m_parent;
	}

	void GameObject::setParent(GameObject* _parent) {
		if (m_parent == _parent)
			return;

		if (m_parent)
			m_parent->removeChild(this);

		m_parent = _parent;

		if(m_parent)
			m_parent->addChild(this);
	}

	void GameObject::Enabled() {
		m_isEnabled = true;
	}

	void GameObject::Disabled() {
		m_isEnabled = false;
	}

	bool GameObject::isEnabled() {
		if (m_parent)
			return m_isEnabled && m_parent->isEnabled();

		return m_isEnabled ;
	}

	void GameObject::Start() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			if (component->m_hasStarted)
				continue;

			component->OnStart();
			component->m_hasStarted = true;
		}
	}

	void GameObject::Update() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnUpdate();
		}
	}

	void GameObject::FixedUpdate() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnFixedUpdate();
		}
	}

	void GameObject::PreRender() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnPreRender();
		}
	}

	void GameObject::Collide() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnCollisionStay();
		}
	}

	void GameObject::Render() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnRender();
		}
	}

	void GameObject::Destroy() {
		if (m_toDestroy)
			return;

		Application::get().addToDestroy(this);

		for (GameObject* child : m_childrens) {
			child->Destroy();
		}

		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnDestroy();
		}
	}

	GameObject::~GameObject() {
		for (auto& [id, component] : m_components) {
			delete component;
		}

		for (GameObject* child : m_childrens) {
			child->setParent(nullptr);
		}
		
		m_parent->removeChild(this);
		m_parent = nullptr;

		m_components.clear();
	}

	void GameObject::removeChild(GameObject* _child) {
		auto it = std::find(m_childrens.begin(), m_childrens.end(), _child);
		m_childrens.erase(it);
	}

	void GameObject::addChild(GameObject* _child) {
		m_childrens.push_back(_child);
	}
}
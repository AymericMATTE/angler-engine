#include "gameObject.h"
#include "application.h"

namespace angler {
	void GameObject::Enabled() {
		m_isEnabled = true;
	}

	void GameObject::Disabled() {
		m_isEnabled = false;
	}

	bool GameObject::isEnabled() {
		return m_isEnabled;
	}

	void GameObject::Start() {
		if (m_isEnabled == false)
			return;

		for (auto& [id, component] : m_components) {
			if (component->m_hasStarted)
				continue;

			component->OnStart();
			component->m_hasStarted = true;
		}
	}

	void GameObject::Update() {
		if (m_isEnabled == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnUpdate();
		}
	}

	void GameObject::FixedUpdate() {
		if (m_isEnabled == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnFixedUpdate();
		}
	}

	void GameObject::PreRender() {
		if (m_isEnabled == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnPreRender();
		}
	}

	void GameObject::Collide() {
		if (m_isEnabled == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnCollisionStay();
		}
	}

	void GameObject::Render() {
		if (m_isEnabled == false)
			return;

		for (auto& [id, component] : m_components) {
			component->OnRender();
		}
	}

	void GameObject::Destroy() {
		if (m_toDestroy)
			return;

		Application::get().addToDestroy(this);

		if (m_isEnabled == false)
			return;
		for (auto& [id, component] : m_components) {
			component->OnDestroy();
		}
	}

	GameObject::~GameObject() {
		for (auto& [id, component] : m_components) {
			delete component;
		}

		m_components.clear();
	}
}
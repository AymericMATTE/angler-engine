#include "pch.h"
#include "game-object.h"
#include "application.h"

namespace angler {
	GameObject* GameObject::getParent() {
		return m_parent;
	}

	void GameObject::setParent(GameObject* _parent) {
		assert(_parent->m_sceneId == m_sceneId);
		if (m_parent == _parent)
			return;

		if (m_parent)
			m_parent->removeChild(this);

		m_parent = _parent;

		if (m_parent) {
			m_parent->addChild(this);
			m_transform.setParent(&_parent->m_transform);
		}
		else {
			m_transform.setParent(nullptr);
		}
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
			component->Start();
		}
	}

	void GameObject::Update() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->Update();
		}
	}

	void GameObject::FixedUpdate() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->FixedUpdate();
		}
	}

	void GameObject::PreRender() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->PreRender();
		}
	}

	void GameObject::Collide(GameObject* _other) {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->CollisionStay(_other);
		}
	}

	void GameObject::Render3D() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->Render3D();
		}
	}

	void GameObject::RenderUI() {
		if (isEnabled() == false)
			return;

		for (auto& [id, component] : m_components) {
			component->RenderUI();
		}
	}

	void GameObject::Destroy() {
		if (m_toDestroy)
			return;

		Application::get().addToDestroy(this);
        m_toDestroy = true;

		for (GameObject* child : m_childrens) {
			child->Destroy();
		}

		for (auto& [id, component] : m_components) {
			component->Destroy();
		}
	}

    void GameObject::setPosition(const DirectX::XMFLOAT3& _position) {
        m_transform.setPosition(_position);
        m_tick++;
    }

    void GameObject::setRotation(const DirectX::XMFLOAT4& _rotation) {
        m_transform.setRotation(_rotation);
        m_tick++;
    }
    void GameObject::setRotation(const DirectX::XMFLOAT3& _rotation) {
        m_transform.setRotation(_rotation);
    }

    void GameObject::setScale(const DirectX::XMFLOAT3& _scale) {
        m_transform.setScale(_scale);
        m_tick++;
    }

    void GameObject::translate(const DirectX::XMFLOAT3& _delta) {
        m_transform.translate(_delta);
        m_tick++;
    }

    void GameObject::rotate(const DirectX::XMFLOAT4& _delta) {
        m_transform.rotate(_delta);
        m_tick++;
    }

    void GameObject::rotate(const DirectX::XMFLOAT3& _rotation) {
        m_transform.rotate(_rotation);
        m_tick++;
    }

    void GameObject::scale(const DirectX::XMFLOAT3& _factor) {
        m_transform.scale(_factor);
        m_tick++;
    }

    void GameObject::setLocalPosition(const DirectX::XMFLOAT3& _position) {
        m_transform.setLocalPosition(_position);
        m_tick++;
    }

    void GameObject::setLocalRotation(const DirectX::XMFLOAT4& _rotation) {
        m_transform.setLocalRotation(_rotation);
        m_tick++;
    }

    void GameObject::setLocalScale(const DirectX::XMFLOAT3& _scale) {
        m_transform.setLocalScale(_scale);
        m_tick++;
    }

    void GameObject::localTranslate(const DirectX::XMFLOAT3& _delta) {
        m_transform.localTranslate(_delta);
        m_tick++;
    }

    void GameObject::localRotate(const DirectX::XMFLOAT4& _delta) {
        m_transform.localRotate(_delta);
        m_tick++;
    }
    void GameObject::localRotate(const DirectX::XMFLOAT3& _delta) {
        m_transform.localRotate(_delta);
        m_tick++;
    }

    void GameObject::localScale(const DirectX::XMFLOAT3& _factor) {
        m_transform.localScale(_factor);
        m_tick++;
    }

    void GameObject::lookAt(const DirectX::XMFLOAT3& _target, const DirectX::XMFLOAT3& up) {
        m_transform.lookAt(_target, up);
        m_tick++;
    }

    void GameObject::lookTo(const DirectX::XMFLOAT3& _direction, const DirectX::XMFLOAT3& up) {
        m_transform.lookTo(_direction, up);
        m_tick++;
    }

    DirectX::XMFLOAT3 GameObject::getPosition() {
        return m_transform.getPosition();
    }

    DirectX::XMFLOAT4 GameObject::getRotation() {
        return m_transform.getRotation();
    }

    DirectX::XMFLOAT3 GameObject::getScale() {
        return m_transform.getScale();
    }

    DirectX::XMFLOAT3 GameObject::getLocalPosition() {
        return m_transform.getLocalPosition();
    }

    DirectX::XMFLOAT4 GameObject::getLocalRotation() {
        return m_transform.getLocalRotation();
    }

    DirectX::XMFLOAT3 GameObject::getLocalScale() {
        return m_transform.getLocalScale();
    }

    DirectX::XMFLOAT4X4 GameObject::getWorldMatrix() {
        return m_transform.getWorldMatrix();
    }

    DirectX::XMFLOAT3 GameObject::getRight() {
        return m_transform.getRight();
    }

    DirectX::XMFLOAT3 GameObject::getUp() {
        return m_transform.getUp();
    }

    DirectX::XMFLOAT3 GameObject::getForward() {
        return m_transform.getForward();
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
        m_transform.removeChild(&_child->m_transform);
	}

	void GameObject::addChild(GameObject* _child) {
		m_childrens.push_back(_child);
        m_transform.addChild(&_child->m_transform);
	}
}
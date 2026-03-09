#pragma once
#include "gameObject.h"

#include <cassert>

#include "componentBase.h"

namespace angler
{
	template<class T>
	T* GameObject::addComponent() {
		//assert(std::is_base_of<Component, T>::value);
		if (m_components.contains(T::StaticId()))
			delete m_components[T::StaticId()];

		T* newComponent = new T();
		newComponent->m_owner = this;
		m_components[newComponent->Id()] = newComponent;
		newComponent->OnAwake();

		return newComponent;
	}

	template<class T>
	T* GameObject::getComponent() {
		//assert(std::is_base_of<Component, T>::value);
		if (m_components.contains(T::StaticId()) == false)
			return nullptr;

		return m_components[T::StaticId()];
	}

	template<class T>
	bool GameObject::hasComponent() {
		return m_components.contains(T::StaticId());
	}

	template<class T>
	void GameObject::removeComponent() {
		//assert(std::is_base_of<Component, T>::value);
		if (m_components.contains(T::StaticId()) == false)
			return;
		Application::get().addToDestroy(m_components[T::StaticId()]);
	}
}
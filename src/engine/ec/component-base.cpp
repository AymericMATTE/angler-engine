#include "pch.h"
#include "component-base.h"
#include "application.h"

using namespace angler;

void Component::Awake() {
	if (isEnabled() == false)
		return;
		
	OnAwake();
}

void Component::Start() {
	if (m_hasStarted)
		return;

	if (isEnabled() == false)
		return;

	m_hasStarted = true;
	OnStart();
}

void Component::Update() {
	if (isEnabled() == false)
		return;

	OnUpdate();
}

void Component::FixedUpdate() {
	if (isEnabled() == false)
		return;

	OnFixedUpdate();
}

void Component::CollisionStay(GameObject* _other) {
	if (isEnabled() == false)
		return;

	OnCollisionStay(_other);
}

void Component::PreRender() {
	if (isEnabled() == false)
		return;

	OnPreRender();
}

void Component::Render3D() {
	if (isEnabled() == false)
		return;

	OnRender3D();
}

void Component::RenderUI() {
	if (isEnabled() == false)
		return;

	OnRenderUI();
}

void Component::Destroy() {
	if (m_toDestroy)
		return;

	Application::get().addToDestroy(this);
	m_toDestroy = true;

	if (isEnabled() == false)
		return;

	OnDestroy();
}

void Component::Enabled() {
	m_isEnabled = true;
}

void Component::Disabled() {
	m_isEnabled = false;
}

bool Component::isEnabled() {
	return m_isEnabled && m_owner->isEnabled();
}
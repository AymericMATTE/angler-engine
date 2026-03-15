#include "pch.h"
#include "collider-component.h"
#include "engine/physics-manager.h"
#include "engine/ec/game-object.h"

namespace angler {
	ColliderComponent::~ColliderComponent(){
		PhysicsManager::get().m_collisionGrid.removeObject(this);
	}

	void ColliderComponent::SetBox(DirectX::XMFLOAT3 _halfExtents, DirectX::XMFLOAT3 _offsetFromTransform) {
		bool firstInit = (m_bounds == nullptr);
		if (firstInit == false) delete m_bounds;
		DirectX::XMFLOAT3 defaultAxes[3] = { DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT3(0, 0, 1) };
		m_bounds = new GameBounds(OBB(DirectX::XMFLOAT3(0, 0, 0), defaultAxes, _halfExtents));
		m_bounds->sphere = new SphereBounds(Physics::sphereFromOBB(m_bounds->box));
		m_baseExtents = _halfExtents;
		m_transformOffset = _offsetFromTransform;
		UpdateBounds();
		PhysicsManager::get().m_collisionGrid.addObject(this);
	}

	void ColliderComponent::SetSphere(float _radius, DirectX::XMFLOAT3 _offsetFromTransform) {
		bool firstInit = (m_bounds == nullptr);
		if (firstInit == false) delete m_bounds;
		m_bounds = new GameBounds(SphereBounds());
		m_baseRadius = _radius;
		m_bounds->sphere->radius = m_baseRadius;
		m_transformOffset = _offsetFromTransform;
		PhysicsManager::get().m_collisionGrid.addObject(this);
	}

	void ColliderComponent::OnFixedUpdate() {
		PhysicsManager::get().m_collisionGrid.updateObject(this);
		std::list<ColliderComponent*> collidingList = PhysicsManager::get().m_collisionGrid.checkObjAgainstGrid(this);
		for (auto other : collidingList)
		{
			getOwner()->Collide(other->getOwner());
		}
	}

	void ColliderComponent::UpdateBounds() {
		if (m_bounds->type == GameBounds::BOX) {
			*m_bounds->sphere = Physics::sphereFromOBB(m_bounds->box);
		}
	}
}
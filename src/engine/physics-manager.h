#pragma once
#include "common/physics.h"
#include "engine/h-grid.h"
#include "engine/ecs/components/collider-component.h"

namespace angler {

class PhysicsManager
{
public:
	HGrid m_collisionGrid;

	static PhysicsManager& get();

private:
	PhysicsManager();
	~PhysicsManager();

	static PhysicsManager* m_instance;
};

}

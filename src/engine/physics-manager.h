#pragma once
#include "common/physics.h"
#include "engine/h-grid.h"
#include "engine/ray.h"

namespace angler {
	class PhysicsManager
	{
	public:
		HGrid m_collisionGrid;

		static PhysicsManager& get();
		GameObject* raycast(Ray* _ray);

	private:
		PhysicsManager();
		~PhysicsManager();

		static PhysicsManager* m_instance;
	};
}

#pragma once
#include "common/physics.h"
#include "engine/h-grid.h"

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

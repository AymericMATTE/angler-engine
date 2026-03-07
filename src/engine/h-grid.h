#pragma once

#include <DirectXMath.h>
#include "common/types.h"
#include "common/physics.h"
#include "ecs/components/collider-component.h"
#include <list>

namespace angler {

#define BUCKET_COUNT 4096
#define LEVEL_COUNT 32
#define SPHERE_TO_CELL_RATIO 1.0f / 4.0f
#define CELL_SCALE_UP 2.0f
#define MIN_CELL_SIZE 0.5f

	class HGrid {
		uint m_occupiedLevelsMask = 0;
		int m_objectCount[LEVEL_COUNT] = { 0 };
		ColliderComponent* m_objectBucket[BUCKET_COUNT] = { nullptr };
		int m_timeStamp[BUCKET_COUNT] = { 0 };
		int m_currentTick;

	public:
		void addObject(ColliderComponent* _obj);
		void removeObject(ColliderComponent* _obj);
		std::list<ColliderComponent*> checkObjAgainstGrid(ColliderComponent* _obj);
		bool isColliding(ColliderComponent* _a, ColliderComponent* _b);
		void resetGrid();

	};


}
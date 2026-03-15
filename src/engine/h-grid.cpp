#include "pch.h"
#include "h-grid.h"
#include "engine/ec/game-object.h"
#include "engine/physics-manager.h"

namespace angler {

	#define sqr(x) (x)*(x)

	int GetHashBucketIndex(const DirectX::XMINT4& _cellPosition) {

		const int h1 = 0x8da6b343; // Large multiplicative constants;
		const int h2 = 0xd8163841; // here arbitrarily chosen primes
		const int h3 = 0xcb1ab31f;
		const int h4 = 0x60000005;

		int n = h1 * _cellPosition.x + h2 * _cellPosition.y + h3 * _cellPosition.z + h4 * _cellPosition.w;

		// Keep between 0 and BUCKET_COUNT
		n = n % BUCKET_COUNT;
		if (n < 0) n += BUCKET_COUNT;

		return n;
	}

	void HGrid::addObject(ColliderComponent* _obj) {

		int level;
		float size = MIN_CELL_SIZE;
		float diameter = 2.0f * _obj->m_bounds->sphere->radius;
		// Find object level
		for (level = 0; size * SPHERE_TO_CELL_RATIO < diameter; level++)
			size *= CELL_SCALE_UP;

		// Assert if object is larger than largest grid cell
		assert(level < LEVEL_COUNT);

		DirectX::XMINT4 cellPos = { (int)(_obj->m_bounds->sphere->center.x / size), (int)(_obj->m_bounds->sphere->center.y / size), (int)(_obj->m_bounds->sphere->center.z / size), level };
		int bucket = GetHashBucketIndex(cellPos);
		_obj->bucket = bucket;
		_obj->level = level;
		_obj->next = m_objectBucket[bucket];
		m_objectBucket[bucket] = _obj;

		m_objectCount[level]++;
		m_occupiedLevelsMask |= (1 << level); // Set level as occupied
	}

	void HGrid::removeObject(ColliderComponent* _obj) {
		m_objectCount[_obj->level]--;
		if (m_objectCount[_obj->level] == 0) // This is the only object of the level
			m_occupiedLevelsMask &= ~(1 << _obj->level); // Set level unused

		int bucket = _obj->bucket;
		ColliderComponent* p = m_objectBucket[bucket];
		// Here, _obj is the header of the cell array
		if (p == _obj) {
			m_objectBucket[bucket] = _obj->next; // Update header
			return;
		}

		// TODO: Check if double linked list or pointer arrays is worth it
		// Find previous object
		while (p) {
			ColliderComponent* q = p;
			p = p->next;
			if (p == _obj) {
				q->next = p->next; // Bypass removed object
				return;
			}
		}

		assert(0); // If we are here, _obj is not in the grid
	}

	void HGrid::updateObject(ColliderComponent* _obj)
	{
		if (_obj->m_tick != _obj->getOwner()->m_tick) {
			PhysicsManager::get().m_collisionGrid.removeObject(_obj);

			DirectX::XMFLOAT3 pos = _obj->getOwner()->getPosition();
			DirectX::XMFLOAT3 scale = _obj->getOwner()->getScale();
			switch (_obj->m_bounds->type) {
			case GameBounds::SPHERE:
			{
				_obj->m_bounds->sphere->center = { pos.x + _obj->m_transformOffset.x, pos.y + _obj->m_transformOffset.y, pos.z + _obj->m_transformOffset.z };
				// Get biggest scale extent
				float maxScaleAxis = scale.x > scale.y ? scale.x : scale.y;
				maxScaleAxis = scale.z > maxScaleAxis ? scale.z : maxScaleAxis;
				_obj->m_bounds->sphere->radius = maxScaleAxis * _obj->m_baseRadius;
				break;
			}

			case GameBounds::BOX:
			{
				_obj->m_bounds->box->center.xmF = { pos.x + _obj->m_transformOffset.x, pos.y + _obj->m_transformOffset.y, pos.z + _obj->m_transformOffset.z };
				_obj->m_bounds->box->axes[0].xmF = _obj->getOwner()->getRight();
				_obj->m_bounds->box->axes[1].xmF = _obj->getOwner()->getUp();
				_obj->m_bounds->box->axes[2].xmF = _obj->getOwner()->getForward();
				DirectX::XMStoreFloat3(&_obj->m_bounds->box->extents.xmF, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&scale), DirectX::XMLoadFloat3(&_obj->m_baseExtents)));
				break;
			}
			}

			//_obj->UpdateBounds();
			_obj->m_tick = _obj->getOwner()->m_tick;

			PhysicsManager::get().m_collisionGrid.addObject(_obj);
		}
	}

	std::list<ColliderComponent*> HGrid::checkObjAgainstGrid(ColliderComponent* _obj)
	{
		float size = MIN_CELL_SIZE;
		int startLevel = 0;
		uint32 occupiedLevelsMask = m_occupiedLevelsMask;
		float diameter = 2.0f * _obj->m_bounds->sphere->radius;

		std::list<ColliderComponent*> res;

		DirectX::XMFLOAT3 pos = _obj->m_bounds->sphere->center;
		// For each new query, increase time stamp counter
		m_currentTick++;
		for (int level = startLevel; level < LEVEL_COUNT;
			size *= CELL_SCALE_UP, occupiedLevelsMask >>= 1, level++) {

			if (occupiedLevelsMask == 0) break; // No more levels occupied
			if ((occupiedLevelsMask & 1) == 0) continue; // Current level inoccupied

			// Compute ranges [x1..x2, y1..y2] of cells overlapped on this level. To
			// make sure objects in neighboring cells are tested, by increasing range by
			// the maximum object overlap: size * SPHERE_TO_CELL_RATIO
			float delta = _obj->m_bounds->sphere->radius + size * SPHERE_TO_CELL_RATIO + EPSILON;
			float ooSize = 1.0f / size;

			int x1 = (int)floorf((pos.x - delta) * ooSize);
			int y1 = (int)floorf((pos.y - delta) * ooSize);
			int z1 = (int)floorf((pos.z - delta) * ooSize);
			int x2 = (int)ceilf((pos.x + delta) * ooSize);
			int y2 = (int)ceilf((pos.y + delta) * ooSize);
			int z2 = (int)ceilf((pos.z + delta) * ooSize);

			DirectX::XMVECTOR posVect = DirectX::XMLoadFloat3(&pos);

			// Check all the grid cells overlapped on current level
			for (int x = x1; x <= x2; x++) {
				for (int y = y1; y <= y2; y++) {
					for (int z = z1; z <= z2; z++) {
						int bucket = GetHashBucketIndex(DirectX::XMINT4(x, y, z, level));

						// Has this hash bucket already been checked for this object?
						if (m_timeStamp[bucket] == m_currentTick) continue;
						m_timeStamp[bucket] = m_currentTick;

						// Loop through all objects in the bucket to find nearby objects
						ColliderComponent* p = m_objectBucket[bucket];
						while (p) {
							if (p == _obj) {
								p = p->next;
								continue;
							}

							float dist2;
							DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(posVect, DirectX::XMLoadFloat3(&p->m_bounds->sphere->center));
							DirectX::XMStoreFloat(&dist2, DirectX::XMVector3Dot(diff, diff));
							if (dist2 <= sqr(_obj->m_bounds->sphere->radius + p->m_bounds->sphere->radius + EPSILON)) {
								if (isColliding(_obj, p)) {
									res.push_back(p);
								}
							}

							p = p->next;
						}
					}
				}
			}
		}

		return res;
	}

	bool HGrid::isColliding(ColliderComponent* _a, ColliderComponent* _b)
	{
		switch (_a->m_bounds->type + _b->m_bounds->type) {
		case GameBounds::SPHERE + GameBounds::SPHERE:
			return Physics::intersects(*_a->m_bounds->sphere, *_b->m_bounds->sphere);
			break;

		case GameBounds::SPHERE + GameBounds::BOX:
			if (_a->m_bounds->type == GameBounds::SPHERE) return Physics::intersects(*_a->m_bounds->sphere, *_b->m_bounds->box);
			else return Physics::intersects(*_a->m_bounds->box, *_b->m_bounds->sphere);
			break;

		case GameBounds::BOX + GameBounds::BOX:
			return Physics::intersects(*_a->m_bounds->box, *_b->m_bounds->box);
			break;
			
		default:
			assert(false);
			return false;
		}
	}
	void HGrid::resetGrid()
	{
		memset(m_objectCount, 0, sizeof(m_objectCount));
		memset(m_objectBucket, 0, sizeof(m_objectBucket));
		memset(m_timeStamp, 0, sizeof(m_timeStamp));

		m_occupiedLevelsMask = 0;
		m_currentTick = 0;
	}
}
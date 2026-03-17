#include "pch.h"
#include "physics-manager.h"

namespace angler{

    PhysicsManager* PhysicsManager::m_instance = nullptr;

    PhysicsManager& PhysicsManager::get() {
        if (m_instance == nullptr) {
            m_instance = new PhysicsManager();
        }
        return *m_instance;
    }

    GameObject* PhysicsManager::raycast(Ray* _ray)
    {
        _ray->updateBounds();

        std::list<ColliderComponent> a = m_collisionGrid.checkObjAgainstGrid(&_ray->bounds);

        return nullptr;
    }

    //bool PhysicsManager::isColliding(ColliderComponent* _a, ColliderComponent* _b)
    //{
    //    return std::find(_a->m_colliding.begin(), _a->m_colliding.end(), _b->m_object) != _a->m_colliding.end(); // TODO: better
    //}

    PhysicsManager::PhysicsManager()
    {
        //m_collisionGrid;
    }

    PhysicsManager::~PhysicsManager()
    {
    }
}

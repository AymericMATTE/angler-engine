#include "pch.h"
#include "physics-manager.h"
#include "engine/inputs/input-manager.h"
#include "engine/ec/game-object.h"

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

        std::list<ColliderComponent*> colliders = m_collisionGrid.checkObjAgainstGrid(_ray->bounds);

        angler::GameObject* closest = nullptr;
        float closestDistance = INFINITY;

        for (auto col : colliders) {
            DirectX::XMFLOAT3 colliderPos = col->getOwner()->getPosition();
            DirectX::XMFLOAT3 diff = { _ray->start.x - colliderPos.x , _ray->start.y - colliderPos.y , _ray->start.z - colliderPos.z };
            float dist = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

            if (dist < closestDistance) {
                closestDistance = dist;
                closest = col->getOwner();
            }
        }

        return closest;
    }

    GameObject* PhysicsManager::raycastFromMouse()
    {
        Ray* ray = new Ray({0, 0, 0}, {1, 0, 0}, 100);
        Ray::raycastFromScreen(ray, InputManager::getMousePos());
        GameObject* go = raycast(ray);
        delete ray;
        return go;
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

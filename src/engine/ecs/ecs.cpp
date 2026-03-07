#include "pch.h"
#include "ecs.h"

namespace angler {
    ECS::Entity ECS::createEntity() {
        return m_next++;
    }
    
    void ECS::destroyEntity(Entity entity) {
        m_toDestroy.push_back(entity);
    }
    
    void ECS::registerSystem(System _system) {
        m_systems[m_sysCount] = _system;
        m_sysCount++;
    }

    void ECS::executeSystems(System::Lifetime _execTime) {
        for (int i = 0; i < m_sysCount; ++i) {
            System& system = m_systems[i];
            if (system.lifetime == _execTime) {
                ECS& self = *this;
                system.func(self);
            }
        }
    }
    
    void ECS::registerScript(Script _script) {
        m_scripts[m_scrCount] = _script;
        m_scrCount++;
    }
    
    void ECS::executeScripts(Script::Lifetime _execTime) {
        for (int i = 0; i < m_scrCount; ++i) {
            Script& script = m_scripts[i];
            if (script.lifetime == _execTime) {
                script.func();
            }
        }
    }
    
    bool ECS::cleanup() {
        if (m_resetRequested) {
            // Skip processing removes/destroys from before reset!
            m_toRemove.clear();
            m_toDestroy.clear();
        
            for (auto& poolR : m_pools) {
                if (poolR) poolR->reset();
            }
            m_next = 1;
            m_resetRequested = false;
            return true;
        }
    
        // Normal cleanup (no reset)
        for (auto compToRemove : m_toRemove) {
            compToRemove.store->remove(compToRemove.entt);
        }
        m_toRemove.clear();
    
        for (auto entity : m_toDestroy) {
            for (auto& pool : m_pools) {
                if (pool) pool->remove(entity);
            }
        }
        m_toDestroy.clear();
    
        return false;
    }

    void ECS::reset() {
        m_resetRequested = true;
    }
}

#pragma once

#include <memory>
#include <array>
#include <vector>

#include "script.h"
#include "system.h"

constexpr uint8_t MAX_COMPONENT_TYPE_COUNT = 32;
constexpr uint8_t MAX_SYSTEMS_COUNT = 16;

#ifdef max
#undef max
#endif

constexpr size_t INVALID_ENTITY = std::numeric_limits<uint32_t>::max();

namespace angler {
    class ECS {
    public:
        using Entity = uint32_t;
        static constexpr size_t invalidEntity = std::numeric_limits<uint32_t>::max();
        
        struct StoreBase {
            virtual ~StoreBase() = default;
            virtual void remove(Entity _entity) = 0;
            virtual bool has(Entity _entity) const = 0;
            virtual void reset() = 0;
        };

        template<typename T, uint32_t MAX_STORE_CAPACITY = 4096>
        struct ComponentStore : StoreBase {
            std::array<Entity, MAX_STORE_CAPACITY> entities;
            std::array<size_t, MAX_STORE_CAPACITY> sparse;
            std::array<T, MAX_STORE_CAPACITY> dense;
            size_t size = 0;
            
            static constexpr size_t invalid = MAX_STORE_CAPACITY;
            
            ComponentStore();
            
            bool has(Entity _entity) const override;
            void remove(Entity _entity) override;
            void reset() override;
        };
        
        template<typename T>
        ComponentStore<T>& getStore();

        Entity createEntity();
        void destroyEntity(Entity entity);
        
        template<typename T>
        bool hasComponent(Entity _entity);
        template<typename T, typename... Args>
        T& addComponent(Entity _entity, Args&&... _args);
        template<typename T>
        void removeComponent(Entity _entity);
        template<typename T>
        T& getComponent(Entity _entity);
        
        template<typename First, typename... Others, typename Func>
        void foreach(Func&& _func);
        
        void registerSystem(System _system);
        void executeSystems(System::Lifetime _execTime);
        
        void registerScript(Script _script);
        void executeScripts(Script::Lifetime _execTime);
        
        bool cleanup();
        void reset();
        
    private:
        std::array<std::unique_ptr<StoreBase>, MAX_COMPONENT_TYPE_COUNT> m_pools;
        std::array<System, MAX_SYSTEMS_COUNT> m_systems;
        std::array<Script, MAX_SYSTEMS_COUNT> m_scripts;
        byte m_sysCount = 0; // TEMP
        byte m_scrCount = 0; // TEMP
        
        bool m_resetRequested = false;
        Entity m_next = 1;
        
        struct CompToRemove {StoreBase* store; Entity entt; };
        std::vector<CompToRemove> m_toRemove;
        std::vector<Entity> m_toDestroy;
    };
}
#include "ecs.inl"
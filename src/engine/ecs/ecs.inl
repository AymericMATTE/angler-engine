#include "ecs.h"

#pragma once

#include <assert.h>

#include "component.h"

namespace angler {
    /// COMPONENT STORE ///

    template <typename T, uint32_t MAX_STORE_CAPACITY>
    ECS::ComponentStore<T, MAX_STORE_CAPACITY>::ComponentStore() {
        sparse.fill(invalid);
    }
    template <typename T, uint32_t MAX_STORE_CAPACITY>
    bool ECS::ComponentStore<T, MAX_STORE_CAPACITY>::has(Entity _entity) const {
        return _entity < sparse.size() && sparse[_entity] != invalid;
    }
    template <typename T, uint32_t MAX_STORE_CAPACITY>
    void ECS::ComponentStore<T, MAX_STORE_CAPACITY>::remove(Entity _entity) {
        if (!has(_entity))
            return;

        size_t index = sparse[_entity];
        size_t lastIndex = size - 1;

        if (index != lastIndex) { // swap with last element if not already last
            dense[index] = std::move(dense[lastIndex]);

            Entity movedEntity = entities[lastIndex];
            entities[index] = movedEntity;
            sparse[movedEntity] = index;
        }
        dense[lastIndex] = {};
        sparse[_entity] = invalid;
        --size;
    }

    template <typename T, uint32_t MAX_STORE_CAPACITY>
    void ECS::ComponentStore<T, MAX_STORE_CAPACITY>::reset() {
        std::fill(entities.begin(), entities.end(), invalidEntity);
        std::fill(sparse.begin(), sparse.end(), invalid);
        std::fill(dense.begin(), dense.end(), T{});
        size = 0;
    }

    ////// CORE ECS ///////

    template <typename T>
    ECS::ComponentStore<T>& ECS::getStore() {
        auto id = Component::get_id<T>(); // get the id of the component from its type (an id is an index in m_pools array)
        
        assert(id < m_pools.size()); // assert if there are more components type than the size of the array
        
        if (!m_pools[id]) { m_pools[id] = std::make_unique<ComponentStore<T>>(); } // create a new pool if the component is not already registered

        return *static_cast<ComponentStore<T>*>(m_pools[id].get()); // lookup the pool, and cast it before returning
    }

    template <typename T>
    bool ECS::hasComponent(Entity _entity) {
        auto& store = getStore<T>(); // get the store of the specified component
        
        return (store.sparse[_entity] != store.invalid); // if the sparse contains a valid index at _entity, than we have this component
    }

    template<typename T, typename... Args>
    T& ECS::addComponent(Entity _entity, Args&&... _args) {
        auto& store = getStore<T>();

        assert(_entity < store.sparse.size()); // entity is not in bound (too many entities)
        assert(store.sparse[_entity] == store.invalid); // entity already have a component of type T associated with it (allow only one component type per entity)
        assert(store.size < store.dense.size()); // we already have too much data in the store (component overflow)

        size_t index = store.size++; // new element is about to get added in the store

        T component(std::forward<Args>(_args)...);
        component.entity = _entity;
        store.dense[index] = component; // construct a component with args
        store.entities[index] = _entity; // set the index to the entity for fast entity lookup
        store.sparse[_entity] = index; // set the sparse to the index for component lookup

        return store.dense[index]; // return the constructed data
    }

    template<typename T>
    void ECS::removeComponent(Entity _entity) {
        auto& store = getStore<T>(); // TO DO : check if the store exist, just in case (don't want to create a new store)
        //store.remove(_entity);
        m_toRemove.push_back({&store, _entity});
    }

    template<typename T>
    T& ECS::getComponent(Entity _entity) {
        auto& store = getStore<T>();

        assert(_entity < store.sparse.size()); // entity is not in bound (invalid entity id or smf)

        size_t index = store.sparse[_entity]; // get the index to lookup associated component
        
        assert(index != store.invalid); // this entity have no components in this store
        assert(store.entities[index] == _entity); // there is a relation problem with the store

        return store.dense[index];
    }

    template <typename First, typename... Others, typename Func>
    void ECS::foreach(Func&& _func) {
        auto& primary_store = getStore<First>();

        for (size_t i = 0; i < primary_store.size; ++i) {
            Entity entity = primary_store.entities[i];

            if ((hasComponent<Others>(entity) && ...)) {

                _func(entity,
                    primary_store.dense[i],
                    getComponent<Others>(entity)...);
            }
        }
    }
}
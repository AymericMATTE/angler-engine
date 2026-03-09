#pragma once
#include "sceneManager.h"

template<class T>
T* SceneManager::CreateScene() {
    T* newScene = new T();

    if (m_freeId.empty()) {
        newScene->m_sceneId = m_scenes.size();
        m_scenes.push_back(newScene);
    }
    else {
        newScene->m_sceneId = m_freeId.front();
        m_scenes[newScene->m_sceneId] = newScene;
        m_freeId.pop();
    }

    return newScene;
}
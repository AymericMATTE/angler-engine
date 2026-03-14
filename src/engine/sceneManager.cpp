#include "sceneManager.h"

namespace angler {
    SceneManager::SceneManager() {
        m_globalScene.m_sceneId = -1;
    }

    Scene* SceneManager::getScene(int _index) {
        assert(_index < m_scenes.size());

        if (_index == -1)
            return &m_globalScene;

        return m_scenes[_index];
    }

    void SceneManager::ChangeScene(Scene* _scene) {
        assert(_scene != nullptr);
        m_activeScene = _scene->m_sceneId;
    }

    void SceneManager::RemoveFromList(Scene* _scene) {
        if (_scene->m_sceneId == -1) // TODO maybe change
            return;

        m_scenes[_scene->m_sceneId] = nullptr;
        m_scenefreeId.push(_scene->m_sceneId);

        if (_scene->m_sceneId != m_activeScene)
            return;

        for (Scene* scene : m_scenes) {
            if (scene == nullptr)
                continue;

            m_activeScene = scene->m_sceneId;
        }
    }

    Scene* SceneManager::getGlobalScene() {
        return &m_globalScene;
    }
}
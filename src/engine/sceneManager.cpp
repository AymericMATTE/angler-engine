#include "sceneManager.h"

namespace angler {
    Scene* SceneManager::getScene(unsigned int _index) {
        assert(_index < m_scenes.size());
        return m_scenes[_index];
    }

    void SceneManager::ChangeScene(Scene* _scene) {
        assert(_scene != nullptr);
        m_activeScene = _scene->m_sceneId;
    }

    void SceneManager::RemoveFromList(Scene* _scene) {
        m_scenes[_scene->m_sceneId] = nullptr;
        m_freeId.push(_scene->m_sceneId);

        if (_scene->m_sceneId != m_activeScene)
            return;

        for (Scene* scene : m_scenes) {
            if (scene == nullptr)
                continue;

            m_activeScene = scene->m_sceneId;
        }
    }
}
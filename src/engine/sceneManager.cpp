#include "sceneManager.h"

Scene* SceneManager::getScene(unsigned int _index) {
    assert(_index < m_scenes.size());
    return m_scenes[_index];
}

Scene* SceneManager::getActiveScene() {
    if (m_activeScene >= m_scenes.size())
        return nullptr;

    return m_scenes[m_activeScene];
}

void SceneManager::ChangeScene(Scene* _scene) {
    m_activeScene = _scene->m_sceneId;
    _scene->OnStart();
}

void SceneManager::RemoveFromList(Scene* _scene) {
    m_scenes[_scene->m_sceneId] = nullptr;
    m_freeId.push(_scene->m_sceneId);

    // TODO Set a new Active Scene if it was the active one
    if (_scene->m_sceneId != m_activeScene)
        return;

    for (Scene* scene : m_scenes) {
        if (scene == nullptr)
            continue;

        m_activeScene = scene->m_sceneId;
        m_scenes[m_activeScene]->OnStart();
    }
}

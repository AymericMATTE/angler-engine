#include "pch.h"
#include "application.h"

#include <fstream>

#include "common.h"
#include "resource-manager.h"
#include "inputs/input-manager.h"
#include "physics-manager.h"
#include "time/clock.h"
#include "utils/os.h"

namespace angler {
    // Remove whitespace from start and end
    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        return s.substr(start, end - start + 1);
    }

    Settings loadSettingsFromFile(const std::string& path) {
        Settings settings;
        std::ifstream file(path);
        if (!file.is_open()) return settings;

        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);

            // Skip comments & empty lines
            if (line.empty() || line.starts_with("--")) continue;

            auto eq = line.find('=');
            if (eq == std::string::npos) continue;

            std::string key = trim(line.substr(0, eq));
            std::string value = trim(line.substr(eq + 1));

            if (key == "window_width") settings.window_width = std::stoi(value);
            else if (key == "window_height") settings.window_height = std::stoi(value);
            else if (key == "window_fullscreen") settings.window_fullscreen = (value == "true");
            else if (key == "vsync") settings.vsync = (value == "true");
            else if (key == "render_scale") settings.render_scale = std::stof(value);
            else if (key == "anti_aliasing") {
                if (value == "NONE") settings.anti_aliasing = NONE;
                else if (value == "MSAA_X2") settings.anti_aliasing = MSAA_X2;
                else if (value == "MSAA_X4") settings.anti_aliasing = MSAA_X4;
                else if (value == "MSAA_X8") settings.anti_aliasing = MSAA_X8;
            }
        }
        return settings;
    } // TEMP

    Application* Application::m_inst = nullptr;
    Settings Application::m_settings;


    Application& Application::get() {
        if (m_inst == nullptr) {
            m_inst = new Application();
        }
        return *m_inst;
    }

    SpriteSheet* logo = nullptr;

    Application::Application() :
        m_window(L"AnglerEngine", m_settings.window_width, m_settings.window_height, m_settings.window_fullscreen),
        m_renderer(m_window, m_settings.render_scale, m_settings.vsync, m_settings.anti_aliasing) {
        m_window.setBackground(stringToWString(os::getFileInDirectory(RES_DIR, "angler-splashscreen.bmp")));
        m_window.setIcon(stringToWString(os::getFileInDirectory(RES_DIR, "favicon.ico")));
        //m_mainCamera = nullptr;

        m_time = 0.0f;
        m_deltaTime = 0.0f;
        m_framerate = 0.0f;
        m_timescale = 1.0f;

        ResourceManager::init(&m_renderer);
    }

    SceneManager& Application::getSceneManager() {
        return m_sceneManager;
    }

    Renderer& Application::getRenderer() {
        return m_renderer;
    }

    Scene* Application::getActiveScene() {
        return m_activeScene;
    }

    float Application::deltaTime() const {
        return m_deltaTime;
    }

    float Application::unscaledDeltaTime() const {
        return m_unscaledDeltaTime;
    }

    float Application::time() const {
        return m_time;
    }

    float Application::unscaledTime() const {
        return m_unscaledTime;
    }

    float Application::framerate() const {
        return m_framerate;
    }

    int Application::run() {
        InputManager::init();

        //REGISTER_SCRIPT(SetupDefaultCameraInputs);
        //REGISTER_SYSTEM(UpdateDefaultCamera);

        //m_ecs.executeScripts(Script::Lifetime::START); // Start scripts
        //m_ecs.executeSystems(System::Lifetime::START); // Start systems

        Clock clock;
        clock.start();

        while (m_window.isOpen()) {
            m_activeScene = m_sceneManager.getScene(m_sceneManager.m_activeScene);

            m_unscaledDeltaTime = static_cast<float>(clock.reset<Clock::seconds>());
            m_deltaTime = m_unscaledDeltaTime * m_timescale;

            m_unscaledTime += m_unscaledDeltaTime;
            m_time += m_deltaTime;

            m_framerate = 1.0f / m_unscaledDeltaTime;
            m_window.setTitle(L"AnglerEngine - FPS : " + std::to_wstring(static_cast<int>(m_framerate)));

            m_window.pollEvents();

            m_renderer.setDeltaTime(m_deltaTime);
            InputManager::update(m_deltaTime, { m_window.getX(), m_window.getY(), m_window.getWidth(), m_window.getHeight() });

        // START

        //UPDATE

        //FIXED UPDATE

        //    // Physics
        //    // Update all Colliders
        //    m_ecs.foreach<ColliderComponent, TransformComponent>([this](ECS::Entity _entity, ColliderComponent& _collider, TransformComponent& _transform) {
        //        if (_collider.m_tick != _transform.m_tick)
        //        {
        //            DirectX::XMFLOAT3 pos = _transform.getPosition();
        //            DirectX::XMFLOAT3 scale = _transform.getScale();
        //            switch (_collider.m_bounds->type) {
        //            case GameBounds::SPHERE:
        //            {
        //                _collider.m_bounds->sphere->center = { pos.x + _collider.m_transformOffset.x, pos.y + _collider.m_transformOffset.y, pos.z + _collider.m_transformOffset.z };
        //                // Get biggest scale extent
        //                float maxScaleAxis = scale.x > scale.y ? scale.x : scale.y;
        //                maxScaleAxis = scale.z > maxScaleAxis ? scale.z : maxScaleAxis;
        //                _collider.m_bounds->sphere->radius = maxScaleAxis * _collider.m_baseRadius;
        //                break;
        //            }

        //            case GameBounds::BOX:
        //            {
        //                _collider.m_bounds->box->center.xmF = { pos.x + _collider.m_transformOffset.x, pos.y + _collider.m_transformOffset.y, pos.z + _collider.m_transformOffset.z };
        //                _collider.m_bounds->box->axes[0].xmF = _transform.getRight();
        //                _collider.m_bounds->box->axes[1].xmF = _transform.getUp();
        //                _collider.m_bounds->box->axes[2].xmF = _transform.getForward();
        //                DirectX::XMStoreFloat3(&_collider.m_bounds->box->extents.xmF, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&scale), DirectX::XMLoadFloat3(&_collider.m_baseExtents)));
        //                break;
        //            }
        //            }

        //            _collider.UpdateBounds();
        //            // Update position in grid:

        //            _collider.m_tick = _transform.m_tick;
        //        }

        //        PhysicsManager::get().m_collisionGrid.addObject(&_collider);
        //        });
        //    // Detect collisions
        //    m_ecs.foreach<ColliderComponent, TransformComponent>([this](ECS::Entity _entity, ColliderComponent& _collider, TransformComponent& _transform) {
        //        std::list<ColliderComponent*> collidingList = PhysicsManager::get().m_collisionGrid.checkObjAgainstGrid(&_collider);

        //        CollisionTagComponent& tag = m_ecs.addComponent<CollisionTagComponent>(_entity);
        //        for (auto other : collidingList)
        //        {
        //            _collider.m_colliding = other;
        //            m_ecs.executeSystems(System::Lifetime::COLLISION); // Start systems
        //        }
        //        tag.test = false;
        //        m_ecs.removeComponent<CollisionTagComponent>(_entity);
        //        _collider.m_colliding = nullptr;

        //        });

            PhysicsManager::get().m_collisionGrid.resetGrid();

            m_renderer.beginFrame();

        // PRERENDER

            // Update main camera
            bool foundMainCamera = false;
        //    m_ecs.foreach<Camera3DComponent, TransformComponent>([this, foundMainCamera](ECS::Entity _entity, Camera3DComponent& _camera3D, TransformComponent& _transform) mutable {
        //        if (!foundMainCamera) {
        //            if (this->m_mainCamera != &_camera3D) {
        //                if (this->m_mainCamera) { this->m_mainCamera->m_isMain = false; }
        //                this->m_mainCamera = &_camera3D;
        //                foundMainCamera = true;
        //            }
        //            else { _camera3D.m_isMain = false; }
        //        }

        //        // Update all camera world matrices
        //        _camera3D.m_camera3D.setTransform(_transform.m_transform); // TODO Optimization
        //        });

        //    // Render lights
        //    m_ecs.foreach<LightComponent, TransformComponent>([this](ECS::Entity _entity, LightComponent& _light, TransformComponent& _transform) {
        //        _light.setPosition(_transform.getPosition());
        //       m_renderer.illuminate(_light.m_light);
        //        });

        //RENDER

        //    // Render all 3D objects
        //    if (m_mainCamera != nullptr) {
                //m_renderer.begin3D(m_mainCamera->m_camera3D);

        //        // Render Meshes
        //        m_ecs.foreach<MeshComponent, TransformComponent>([this](ECS::Entity _entity, MeshComponent& _mesh, TransformComponent& _transform) {
        //            if (_mesh.m_mesh == nullptr || _mesh.m_material == nullptr) return;
        //            m_renderer.draw(*_mesh.m_mesh, _transform.m_transform, *_mesh.m_material);
        //            });

        //        // Render Particles
        //        m_ecs.foreach<ParticleEmitterComponent, TransformComponent>([this](ECS::Entity _entity, ParticleEmitterComponent& _em, TransformComponent& _transform) {
        //            _em.m_emitter->setPosition(_transform.getPosition());
        //            m_renderer.drawEmitter(*_em.m_emitter);
        //            });

            m_renderer.end3D();
        //    }

            // m_renderer.begin2D();
            // m_renderer.end2D();

            m_renderer.beginUI();

        //    m_ecs.foreach<SpriteComponent, TransformComponent>([this](ECS::Entity _entity, SpriteComponent& _sprite, TransformComponent& _transform) {
        //        m_renderer.drawSprite(_sprite.getSprite(), _sprite.getPosition(), _sprite.getLayer(), _sprite.getAnchor(), _sprite.getScale(), _sprite.getRotation(), _sprite.getColor());
        //        });

        //    m_ecs.foreach<TextComponent, TransformComponent>([this](ECS::Entity _entity, TextComponent& _text, TransformComponent& _transform) {
        //        m_renderer.setFont(_text.getFont());
        //        m_renderer.drawText(_text.getString().c_str(), _text.getPosition(), _text.getLayer(), _text.getAnchor(), DirectX::XMFLOAT2(_text.getScale(), _text.getScale()), _text.getRotation(), _text.getColor());
        //        });

            m_renderer.endUI();

            m_renderer.endFrame();

            cleanup();
        }
        return 0;
    }

    void Application::setTimescale(float _timeScale) {
        m_timescale = _timeScale;
    }

    float Application::getTimescale() const {
        return m_timescale;
    }

    Settings Application::getSettings() const {
        return m_settings;
    }

    void Application::addToDestroy(GameObject* _gameObject) {
        m_goToDestroy.push_back(_gameObject);
    }

    void Application::addToDestroy(Scene* _scene) {
        m_sceneToDestroy.push_back(_scene);
    }

    void Application::addToDestroy(Component* _component) {
        m_componentToDestroy.push_back(_component);
    }

    void Application::cleanup() {
        while (m_componentToDestroy.empty() == false)
        {
            Component* component = m_componentToDestroy.back();
            component->getOwner()->m_components.erase(component->Id());
            delete component;
            m_componentToDestroy.pop_back();
        }

        while (m_goToDestroy.empty() == false)
        {
            GameObject* gameObject = m_goToDestroy.back();
            Scene* scene = m_sceneManager.getScene(gameObject->m_sceneId);
            scene->RemoveFromList(gameObject);
            delete gameObject;
            m_goToDestroy.pop_back();
        }

        while (m_sceneToDestroy.empty() == false)
        {
            Scene* scene = m_sceneToDestroy.back();
            m_sceneManager.RemoveFromList(scene);
            delete scene;
            m_sceneToDestroy.pop_back();
        }
    }
}
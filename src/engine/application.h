#pragma once

// Local dependencies
#include "ecs/ecs.h"
#include "ecs/components/camera-3d-component.h"
#include "render/os/window.h"
#include "render/renderer/renderer.h"
#include "common/event/event.h"

#define APPLICATION Application::get()

namespace angler {
    enum AntiAliasing : uint {
        NONE        = 1,
        MSAA_X2     = 2,
        MSAA_X4     = 3,
        MSAA_X8     = 4
    };
    
    struct Settings {
        int window_width = 960;
        int window_height = 540;
        bool window_fullscreen = false;
        
        bool vsync = true;
        float render_scale = 1.0f;
        AntiAliasing anti_aliasing = MSAA_X4;
    };

    class Application {
    public:
        static Application& get();
        
        ECS& getECS();
        Renderer& getRenderer();
        
        float deltaTime() const;
        float unscaledDeltaTime() const;
        
        float time() const;
        float unscaledTime() const;
        
        float framerate() const;
        
        int run();
        
        void setTimescale(float _timeScale);
        float getTimescale() const; 
        
        Settings getSettings() const;
        
        Event<> OnApplicationReset;

    private:
        Application();
        static Application* m_inst;

        Camera3DComponent* m_mainCamera;
        
        float m_deltaTime;
        float m_unscaledDeltaTime;
        
        float m_time;
        float m_unscaledTime;
        
        float m_framerate;
        float m_timescale;
        
        ECS m_ecs;
        Window m_window;
        Renderer m_renderer;
        
        static Settings m_settings;
    };
}
#include "application.inl"
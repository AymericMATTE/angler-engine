#pragma once
#include "sceneManager.h"
#include "render/os/window.h"
#include "render/renderer/renderer.h"

namespace angler
{
	enum AntiAliasing : uint {
		NONE = 1,
		MSAA_X2 = 2,
		MSAA_X4 = 3,
		MSAA_X8 = 4
	};

	struct Settings {
		int window_width = 960;
		int window_height = 540;
		bool window_fullscreen = false;

		bool vsync = true;
		float render_scale = 1.0f;
		AntiAliasing anti_aliasing = MSAA_X4;
	};

	class Application
	{
	public :
		static Application& get();

		SceneManager& getSceneManager();
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

		Scene* getActiveScene();

		void addToDestroy(GameObject* _gameObject);
		void addToDestroy(Scene* _scene);
		void addToDestroy(Component* _component);

		void cleanup();

	private :
		Application();
		static Application* m_inst;
		static Settings m_settings;
		SceneManager m_sceneManager;
		Scene* m_activeScene = nullptr;

		float m_deltaTime;
		float m_unscaledDeltaTime;

		float m_time;
		float m_unscaledTime;

		float m_framerate;
		float m_timescale;

		Window m_window;
		Renderer m_renderer;

		std::vector<GameObject*> m_goToDestroy;
		std::vector<Scene*> m_sceneToDestroy;
		std::vector<Component*> m_componentToDestroy;
	};
}
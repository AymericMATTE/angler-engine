#pragma once
#include "engine/scene.h"
#include "engine/sceneManager.h"

class SandboxScene: public angler::Scene
{
public:
	void OnStart();
	void OnUpdate();

	angler::GameObject* raycastTastObject;

	friend class SceneManager;
};


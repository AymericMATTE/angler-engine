#include "pch.h"
#include "main.h"

// Override engine settings 
// needs to be done before including app :)
#define ANGLER_WINDOW_WIDTH 1920
#define ANGLER_WINDOW_HEIGHT 1080
#include "application.h"

#pragma comment(linker, "/WHOLEARCHIVE:engine.lib") // DO NOT REMOVE OR AUTOMATIC SCRIPTS & SYSTEM REGISTRATION WILL FAIL, signed Ethan. thanks

#include <DirectXMath.h>

#include "render/os/window.h"
#include "resource-manager.h"
#include "sandbox-scene.h"
#include "common/time/clock.h"
//#include "ec/components/default-camera-component.h"
#include "render/dx12/objects/mesh.h"
#include "render/renderer/renderer.h"
//#include "ec/components/mesh-component.h"
//#include "ec/components/light-component.h"
//#include "ec/components/particle-emitter-component.h"
//#include "ec/components/sprite-component.h"
//#include "ec/components/text-component.h"
//#include "ec/components/transform-component.h"
#include "render/renderer/cameras/camera-3d.h"
#include "render/dx12/shaders/compute-shader.h"
#include "render/dx12/root-signatures/compute-root-sig.h"
#include "render/dx12/particles-emitter/particles-emitter.h"

using namespace angler;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    Application& app = Application::get();

    Renderer& renderer = app.getRenderer();
    Scene* sandboxScene = app.getSceneManager().CreateScene<SandboxScene>();
    app.getSceneManager().ChangeScene(sandboxScene);
    
    return app.run();
}
#include "pch.h"
#include "main.h"

// Override engine settings 
// needs to be done before including app :)
#define ANGLER_WINDOW_WIDTH 1920
#define ANGLER_WINDOW_HEIGHT 1080
#include "application.h"
#include "game-resources.h"

#include "game-manager.h"
#include "generation/lvl-generator.h"
#include "generation/rail-system.h"
#include "player.h"
#include "bullet.h"
#include "qte.h"
#include "enemy.h"

using namespace angler;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    Application& app = APPLICATION;
    
    ECS& ecs = app.getECS();
    Renderer& renderer = app.getRenderer();
    
    REGISTER_SCRIPT(SetupGameInputs);
    REGISTER_SCRIPT(GameStartup);
    REGISTER_SCRIPT(GameManagerStart); 
    REGISTER_SCRIPT(GameManagerUpdate);

    REGISTER_SYSTEM(RailSystem);
    REGISTER_SYSTEM(ChunkCleanupSystem);

    REGISTER_SYSTEM(PlayerUpdate);
    REGISTER_SYSTEM(PlayerCameraUpdate);

    REGISTER_SYSTEM(BulletCollide);
    REGISTER_SYSTEM(BulletUpdate);

    REGISTER_SYSTEM(ABQTEUpdate);
    
    REGISTER_SYSTEM(enemyRotation);

    REGISTER_SYSTEM(enemyBulletUpdate);
    REGISTER_SYSTEM(enemyBulletCollide);

    return app.run();
}
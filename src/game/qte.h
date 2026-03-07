#pragma once

#include "engine/ecs/system.h"
#include "engine/application.h"

namespace angler {
    struct SpriteSheet;
}

struct ABQTEComponent: public angler::ComponentBase {
    bool active;
    bool completed = false;
    int maxPressCount = 8;
    int currentPressCount = 0;
    bool aPressed = false;

    angler::ECS::Entity aButton;
    angler::ECS::Entity bButton;
    angler::ECS::Entity popupWindow;

    angler::SpriteSheet* pixelFont = nullptr;
    angler::SpriteSheet* popupSprite = nullptr;
};

angler::ECS::Entity createPopupWindow(float _vehicleDamagePercent);

void removePopupWindow(angler::ECS::Entity _window);

void ABQTEUpdateFunction(angler::ECS::Entity _ent, ABQTEComponent& _qte);

SYSTEM_BEGIN(ABQTEUpdate, System::UPDATE, ABQTEComponent)
([](angler::ECS::Entity _ent, ABQTEComponent& _qte) {
    ABQTEUpdateFunction(_ent, _qte);
    });
SYSTEM_END()

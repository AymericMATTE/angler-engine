#include "pch.h"
#include "qte.h"

#include "engine/ecs/components/mesh-component.h"
#include "engine/ecs/components/collider-component.h"
#include "engine/ecs/components/transform-component.h"
#include "engine/ecs/components/sprite-component.h"
#include "engine/ecs/components/text-component.h"
#include "engine/inputs/input-manager.h"
#include "engine/resource-manager.h"
#include "game/game-resources.h"

using namespace angler;

ECS::Entity createPopupWindow(float _vehicleDamagePercent)
{
    ECS& ecs = Application::get().getECS();
    Settings settings = APPLICATION.getSettings();

    ECS::Entity window = ecs.createEntity();

    const DirectX::XMINT2 windowSize = { 300, 200 };
    int x = rand() % (settings.window_width - windowSize.x) + (windowSize.x / 2);
    int y = rand() % (settings.window_height - windowSize.y) + (windowSize.y / 2);
    DirectX::XMFLOAT2 windowPos = { (float)x , (float)y };

    TransformComponent& windowTransform = ecs.addComponent<TransformComponent>(window);
    SpriteComponent& windowSprite = ecs.addComponent<SpriteComponent>(window);
    ABQTEComponent& windowQTE = ecs.addComponent<ABQTEComponent>(window);
    windowQTE.active = true;
    windowQTE.maxPressCount = static_cast<int>(24.0f * _vehicleDamagePercent);

    windowSprite.setSprite(g_res.popupSprite);
    windowSprite.setPosition(windowPos);
    windowSprite.setAnchor({ 0.5f, 0.5f });

    windowSprite.setScale({ static_cast<float>(windowSize.x) / static_cast<float>(windowSprite.getSprite()->spriteSize.x), static_cast<float>(windowSize.y) / static_cast<float>(windowSprite.getSprite()->spriteSize.y) });
    windowSprite.setLayer(4);

    int buttonOffset = static_cast<int>(windowSize.x * 0.2f);
    float activeButtonScale = windowSize.x * 0.15f / 64;
    float baseButtonScale = windowSize.x * 0.075f / 64;

    windowQTE.aButton = ecs.createEntity();
    TransformComponent& aButtonTransform = ecs.addComponent<TransformComponent>(windowQTE.aButton);
    TextComponent& aButtonText = ecs.addComponent<TextComponent>(windowQTE.aButton);
    aButtonText.setString("Q/A");
    aButtonText.setPosition({ windowPos.x - buttonOffset, windowPos.y });
    aButtonText.setFont(g_res.mainFont);
    aButtonText.setAnchor({ 0.5f, 0.5f });
    aButtonText.setScale(activeButtonScale);
    aButtonText.setLayer(5);

    windowQTE.bButton = ecs.createEntity();
    TransformComponent& bButtonTransform = ecs.addComponent<TransformComponent>(windowQTE.bButton);
    TextComponent& bButtonText = ecs.addComponent<TextComponent>(windowQTE.bButton);
    bButtonText.setString("D/B");
    bButtonText.setPosition({ windowPos.x + buttonOffset, windowPos.y });
    bButtonText.setFont(g_res.mainFont);
    bButtonText.setAnchor({ 0.5f, 0.5f });
    bButtonText.setScale(baseButtonScale);
    bButtonText.setLayer(5);

    return window;
}

void removePopupWindow(ECS::Entity _window)
{
    ECS& ecs = APPLICATION.getECS();
    ABQTEComponent& qte = ecs.getComponent<ABQTEComponent>(_window);
    ecs.destroyEntity(qte.aButton);
    ecs.destroyEntity(qte.bButton);
    ecs.destroyEntity(qte.popupWindow);
    ecs.destroyEntity(_window);
}

void ABQTEUpdateFunction(ECS::Entity _ent, ABQTEComponent& _qte)
{
    if (_qte.active == false)
        return;
    ECS& ecs = APPLICATION.getECS();

    const DirectX::XMINT2 windowSize = { 300, 200 };
    float activeButtonScale = windowSize.x * 0.15f / 64;
    float baseButtonScale = windowSize.x * 0.075f / 64;

    if (_qte.aPressed == false) {
        if (InputManager::getActionDown("qte-button-1")) {
            ecs.getComponent<TextComponent>(_qte.aButton).setScale(baseButtonScale);
            ecs.getComponent<TextComponent>(_qte.bButton).setScale(activeButtonScale);

            _qte.aPressed = true;
        }
    }

    else {
        if (InputManager::getActionDown("qte-button-2")) {
            ecs.getComponent<TextComponent>(_qte.aButton).setScale(activeButtonScale);
            ecs.getComponent<TextComponent>(_qte.bButton).setScale(baseButtonScale);

            _qte.aPressed = false;
            _qte.currentPressCount++;
        }
    }

    // Completed
    if (_qte.currentPressCount >= _qte.maxPressCount) {
        _qte.completed = true;
        _qte.active = false;
    }

}

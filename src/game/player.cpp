#include "pch.h"
#include "player.h"
#include "bullet.h"
#include "physics-manager.h"
#include "resource-manager.h"
#include "game/game-resources.h"
#include "engine/inputs/input-manager.h"
#include "engine/ecs/components/mesh-component.h"
#include "engine/ecs/components/particle-emitter-component.h"
#include "game/generation/rail-follower-component.h"
#include "game/qte.h"

#include <algorithm>

using namespace angler;

void createBullet(PlayerComponent& _player) {
    ECS& ecs = Application::get().getECS();
    TransformComponent& transform = ecs.getComponent<TransformComponent>(_player.entity);
    DirectX::XMFLOAT3 fwd = transform.getForward();
    DirectX::XMFLOAT3 pos = transform.getPosition();

    ECS::Entity bullet = ecs.createEntity();
    TransformComponent& bulletTransform = ecs.addComponent<TransformComponent>(bullet);
    bulletTransform.setPosition({ pos.x + fwd.x, pos.y + fwd.y, pos.z + fwd.z });
    bulletTransform.lookTo(fwd);
    bulletTransform.setScale({ 0.05f, 0.05f, 0.05f });

    ecs.addComponent<BulletComponent>(bullet).speed = 50.f;
    ecs.addComponent<ColliderComponent>(bullet).SetSphere();

    MeshComponent& bulletMesh = ecs.addComponent<MeshComponent>(bullet);

    bulletMesh.setMesh(g_res.bulletMesh);
    bulletMesh.setMaterial(g_res.bulletMaterial);
}

void playerCameraUpdateFunction(ECS::Entity _ent, PlayerCameraPivotComponent& _playerCam, TransformComponent& _transform) {
    ECS& ecs = Application::get().getECS();
    const float delta = APPLICATION.deltaTime();

    // Mouse look
    const float mouseX = InputManager::getAxis("default_cam_mouse_x");
    const float mouseY = InputManager::getAxis("default_cam_mouse_y");

    TransformComponent& transformCamera = ecs.getComponent<TransformComponent>(_playerCam.player);

    float yRot = mouseX * delta * _playerCam.rotationFactor;
    _transform.localRotate({ 0.0f, yRot, 0.0f });

    // X Axis Clamp
    DirectX::XMFLOAT4 cameraQuat = _transform.getLocalRotation();
    DirectX::XMFLOAT3 up = { 0, 1, 0 };
    DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);
    float angleY;
    DirectX::XMQuaternionToAxisAngle(&upVec, &angleY, DirectX::XMLoadFloat4(&cameraQuat));

    if (angleY > (3.1415f / 2)) { // This sucks but can't find a better implementation
        _transform.localRotate({ 0.0f, -yRot, 0.0f });
    }

    // Y Axis Clamp
    float xRot = -(mouseY * delta * _playerCam.rotationFactor);
    xRot = transformCamera.getForward().y - xRot >= _playerCam.xMax ? std::max(transformCamera.getForward().y - _playerCam.xMax, xRot) : xRot;
    xRot = transformCamera.getForward().y - xRot <= _playerCam.xMin ? std::min(transformCamera.getForward().y - _playerCam.xMin, xRot) : xRot;

    transformCamera.localRotate({ xRot, 0.0f, 0.0f });
}

void playerUpdateFunction(ECS::Entity _ent, PlayerComponent& _player, TransformComponent& _transform) {
    ECS& ecs = Application::get().getECS();
    float delta = APPLICATION.deltaTime();

    // TIMERS
    _player.shotChrono -= delta;
    _player.reloadChrono -= delta;
    _player.particleUpdateChrono -= delta;

    if (_player.reloading && _player.reloadChrono <= 0) {
        _player.reloading = false;
        ecs.getComponent<TransformComponent>(_player.weapon).localTranslate({ 0.05f, 0.0f, 0.0f});
        ecs.getComponent<TransformComponent>(_player.weapon).localRotate({ 0.0f, 0.0f, -3.1415f / 4.0f });
    }

    if (_player.repairing) {
        if (ecs.getComponent<ABQTEComponent>(_player.QTEWindow).completed) {
            removePopupWindow(_player.QTEWindow);
            _player.vehicleHealth = _player.maxVehicleHealth;
            _player.repairing = false;
        }
    }

    // INPUTS
    if (InputManager::getAction("duck") && 
        _player.repairing == false &&
        _player.maxVehicleHealth > 0) {
        _player.heightOffset = 0.1f;
        _player.dunking = true;
    }
    else {
        _player.heightOffset = 0.5f;
        _player.dunking = false;
    }

    if (InputManager::getAction("reload") &&
        _player.reloading == false &&
        _player.repairing == false) {
        _player.reloadChrono = _player.reloadTime;
        _player.reloading = true;
        _player.bulletCount = _player.maxBulletCount;
        ecs.getComponent<TransformComponent>(_player.weapon).localRotate({0.0f, 0.0f, 3.1415f / 4.0f});
        ecs.getComponent<TransformComponent>(_player.weapon).localTranslate({ -0.05f, 0.0f, 0.0f });

    }

    if (InputManager::getActionDown("shoot")&&
        _player.reloading == false &&
        _player.dunking == false && 
        _player.repairing == false &&
        _player.bulletCount > 0 &&
        _player.shotChrono <= 0) {

        createBullet(_player);
        _player.shotChrono = _player.shotCooldown;
        _player.bulletCount--;
    }

    if (InputManager::getActionDown("repair") &&
        _player.reloading == false &&
        _player.repairing == false &&
        _player.vehicleHealth < _player.maxVehicleHealth) {
        _player.repairing = true;
        float damageRatio = (float)(_player.maxVehicleHealth - _player.vehicleHealth) / _player.maxVehicleHealth;
        _player.QTEWindow = createPopupWindow(damageRatio);
    }

    _player.vehicleHealth = std::max(_player.vehicleHealth, 0);

    float speedFactor = 0.5f + (static_cast<float>(_player.vehicleHealth) / static_cast<float>(_player.maxVehicleHealth)) * 0.5f;
    float vehicleYOffsetTarget = -0.5f + (static_cast<float>(_player.vehicleHealth) / static_cast<float>(_player.maxVehicleHealth)) * 0.5f;
    float vehicleYOffset = _player.vehicleFollower->offsetPosition.y + (vehicleYOffsetTarget - _player.vehicleFollower->offsetPosition.y) * 2.f * delta;

    if (_player.vehicleHealth == 0) {
        speedFactor = 0;
    }

    _player.vehicleFollower->speed = std::lerp(_player.minSpeed, _player.maxSpeed, speedFactor);
    _player.vehicleFollower->offsetPosition = { 0, vehicleYOffset, 0 };

    // Particles
    ecs.getComponent<TransformComponent>(_player.vehicleEmitter->entity).setLocalPosition({ 0, -0.2f, 0.5f * _player.maxSpeed * speedFactor });
    if (_player.particleUpdateChrono <= 0) {
        float particleFactor = (float)(_player.maxVehicleHealth - _player.vehicleHealth)/ _player.maxVehicleHealth;


        _player.particleUpdateChrono = _player.particleUpdateCooldown;
        _player.vehicleEmitter->setScaleMax(0.1f * particleFactor);
        _player.vehicleEmitter->setParticlesCount(200 * static_cast<int32>(particleFactor) + 1);
        _player.vehicleEmitter->initialize();
        _player.vehicleEmitter->start();
    }
    

    float currentHeight = _transform.getLocalPosition().y + (_player.heightOffset - _transform.getLocalPosition().y) * 16.f * delta;
    _transform.setLocalPosition({ 0, currentHeight, 0 });
}

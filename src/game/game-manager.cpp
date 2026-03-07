#include "pch.h"
#include "game-manager.h"
#include "engine/inputs/input-manager.h"
#include "engine/ecs/components/transform-component.h"
#include "engine/ecs/components/collider-component.h"
#include "engine/ecs/components/light-component.h"
#include "engine/ecs/components/text-component.h"
#include "engine/ecs/components/sprite-component.h"
#include "engine/ecs/components/mesh-component.h"
#include "engine/ecs/components/particle-emitter-component.h"
#include "render/dx12/objects/static-mesh.h"
#include "player.h"
#include "game-resources.h"
#include "resource-manager.h"
#include "generation/lvl-generator.h"
#include "game-state.h"
#include "ecs/components/default-camera-component.h"
#include "generation/rail-follower-component.h"
#include "generation/lvl-generator.h"

#include "game/enemy.h"

void setupGameInputsFunction() {
	InputManager::addAction("shoot", {
	Keyboard::Key::NONE,
	Mouse::Key::BUTTON_LEFT,
	Gamepad::Key::RIGHT_TRIGGER_BTN
	});
InputManager::addAction("duck", {
	Keyboard::Key::LCONTROL,
	Mouse::Key::NONE,
	Gamepad::Key::LEFT_TRIGGER_BTN
	});
InputManager::addAction("qte-button-1", {
	Keyboard::Key::Q,
	Mouse::Key::NONE,
	Gamepad::Key::A
	});
InputManager::addAction("qte-button-2", {
	Keyboard::Key::D,
	Mouse::Key::NONE,
	Gamepad::Key::B
	});
InputManager::addAction("reload", {
	Keyboard::Key::R,
	Mouse::Key::NONE,
	Gamepad::Key::RIGHT_SHOULDER
	});
InputManager::addAction("repair", {
	Keyboard::Key::F,
	Mouse::Key::NONE,
	Gamepad::Key::LEFT_SHOULDER
	});
InputManager::addAction("restart", {
	Keyboard::Key::L,
	Mouse::Key::NONE,
	Gamepad::Key::START
	});
	Mouse::lock();
}

void resetGameState() {
	s_nextGeneration = 0;
	g_state = {};
	g_res = GameResources();
	APPLICATION.setTimescale(1.0f);
}

void gameStartupFunction() {
	Application& app = APPLICATION;
	ECS& ecs = app.getECS();
	Renderer& renderer = app.getRenderer();
	
	app.OnApplicationReset.connect(&resetGameState);
	
	// Load game resources
	g_res.load(renderer);

	////// LVL GENERATION //////
	ECS::Entity first = spawnFirstChunk();
	ECS::Entity currentEnt = first;
	srand(0);
	for (int i = 0; i < 1; ++i) {
		int randomType = rand() % 3;
		currentEnt = spawnNextChunk(randomType, currentEnt);
	}

	// Spawn coffin
	ECS::Entity coffinEntity = ecs.createEntity();
	TransformComponent& coffinTransformComp = ecs.addComponent<TransformComponent>(coffinEntity);
    
	ECS::Entity waterEmitterEntity = ecs.createEntity();
	TransformComponent& waterTransform = ecs.addComponent<TransformComponent>(waterEmitterEntity);
	ParticleEmitterComponent& waterParticles = ecs.addComponent<ParticleEmitterComponent>(waterEmitterEntity);
	waterParticles.addParticleColor({ 0.2f, 0.8f, 0.3f, 1.0f });
	waterParticles.setMesh(ResourceManager::getMesh("mesh-quad"));
	waterParticles.setSpeedMin(2.5f);
	waterParticles.setSpeedMax(5);
	waterParticles.setBillBoardMode(BillBoard::Spheric);
	waterParticles.setSpawnRadius(0.25f);
	waterParticles.setLifetimeMin(0.25f);
	waterParticles.setLifetimeMax(0.5f);
	waterParticles.setParticlesCount(1);
	waterParticles.setRespawnTimeMin(1.0f);
	waterParticles.setRespawnTimeMin(1.5f);
	waterParticles.setScaleMin(0.0f);
	waterParticles.setScaleMax(0.0f);
	waterParticles.initialize();
	waterParticles.start();
	waterTransform.setParent(&coffinTransformComp);

	RailFollower& rail = ecs.addComponent<RailFollower>(coffinEntity);
	rail.speed = 3.0f;
	rail.active = true;
    
	MeshComponent& coffinMeshComp = ecs.addComponent<MeshComponent>(coffinEntity);
	coffinMeshComp.setMesh(g_res.coffinMesh);
	coffinMeshComp.setMaterial(g_res.coffinMaterial);

	coffinTransformComp.setLocalScale({ 1.0f, 1.0f, 1.0f });
	
	g_state.vehicle = coffinEntity;

	// Pivot // 
	ECS::Entity cameraPivotEntity = ecs.createEntity();
	PlayerCameraPivotComponent& cameraPivotComp = ecs.addComponent<PlayerCameraPivotComponent>(cameraPivotEntity);
	TransformComponent& cameraPivotTransform = ecs.addComponent<TransformComponent>(cameraPivotEntity);
	cameraPivotTransform.setLocalPosition({0, 0.5f, 0});
	cameraPivotTransform.setParent(&coffinTransformComp);

	// Player //
	g_state.player = ecs.createEntity();
	PlayerComponent& playerComp = ecs.addComponent<PlayerComponent>(g_state.player);
	TransformComponent& playerTransformComp = ecs.addComponent<TransformComponent>(g_state.player);
	ColliderComponent& playerColliderComp = ecs.addComponent<ColliderComponent>(g_state.player);
	playerColliderComp.SetSphere(1.f);
	cameraPivotComp.player = g_state.player;
	Camera3DComponent& camera3DComp = ecs.addComponent<Camera3DComponent>(g_state.player);
	playerComp.vehicleFollower = &rail;
	playerComp.vehicleEmitter = &waterParticles;
	
	playerTransformComp.setLocalPosition({ 0, 0.5f, 0 });
	playerTransformComp.setParent(&cameraPivotTransform);

	// Gun //
	ECS::Entity gunEntity = ecs.createEntity();
	TransformComponent& gunTransformComp = ecs.addComponent<TransformComponent>(gunEntity);
	gunTransformComp.setParent(&playerTransformComp);
	gunTransformComp.setScale({0.25f, 0.25f, 0.25f});
	gunTransformComp.setLocalPosition({ 0.075f, -0.07f, 0.15f });
	MeshComponent& gunMeshComp = ecs.addComponent<MeshComponent>(gunEntity);
	gunMeshComp.setMesh(g_res.gunMesh);
	gunMeshComp.setMaterial(g_res.gunMaterial);
	playerComp.weapon = gunEntity;

	// Debug //
	LightComponent& sun = ecs.addComponent<LightComponent>(g_state.player);
	sun.directional();
	sun.setIntensity(2.0f);
}

void gameManagerStartFunction() {
	ECS& ecs = APPLICATION.getECS();
	
	DirectX::XMFLOAT2 windowSize = DirectX::XMFLOAT2(static_cast<float>(APPLICATION.getSettings().window_width), static_cast<float>(APPLICATION.getSettings().window_height));
	
	// SCORE //
	ECS::Entity scoreEnt = ecs.createEntity();
	ecs.addComponent<TransformComponent>(scoreEnt);
	g_state.scoreText = &ecs.addComponent<TextComponent>(scoreEnt);
	g_state.scoreText->setPosition({ windowSize.x * 0.5f , windowSize.y * 0.05f });
	g_state.scoreText->setScale(0.5f);
	g_state.scoreText->setAnchor({0.5f, 0.5f});
	g_state.scoreText->setFont(g_res.mainFont);
	g_state.scoreText->setString("Distance Travelled: ");
	g_state.scoreText->setLayer(1);

	ECS::Entity bulletEnt = ecs.createEntity();
	ecs.addComponent<TransformComponent>(bulletEnt);
	g_state.bulletText = &ecs.addComponent<TextComponent>(bulletEnt);
	g_state.bulletText->setPosition({ windowSize.x * 0.08f , windowSize.y * 0.87f });
	g_state.bulletText->setScale(0.5f);
	g_state.bulletText->setAnchor({0.5f, 0.5f});
	g_state.bulletText->setFont(g_res.mainFont);
	g_state.bulletText->setLayer(1);
	
	ECS::Entity overScoreEnt = ecs.createEntity();
	ecs.addComponent<TransformComponent>(overScoreEnt);
	g_state.gameOverScoreText = &ecs.addComponent<TextComponent>(overScoreEnt);
	g_state.gameOverScoreText->setPosition({ windowSize.x * 0.5f , windowSize.y * 0.5f });
	g_state.gameOverScoreText->setScale(0);
	g_state.gameOverScoreText->setAnchor({0.5f, 0.5f});
	// TODO: font loading macro?
	g_state.gameOverScoreText->setFont(g_res.mainFont);
	g_state.gameOverScoreText->setLayer(7);

	ECS::Entity restartEnt = ecs.createEntity();
	ecs.addComponent<TransformComponent>(restartEnt);
	g_state.restartText = &ecs.addComponent<TextComponent>(restartEnt);
	g_state.restartText->setPosition({ windowSize.x * 0.5f , windowSize.y * 0.75f });
	g_state.restartText->setScale(0);
	g_state.restartText->setAnchor({0.5f, 0.5f});
	// TODO: font loading macro?
	g_state.restartText->setFont(g_res.mainFont);
	g_state.restartText->setLayer(7);
	g_state.restartText->setString("To restart, press L/Start.");

	ECS::Entity crosshairEnt = ecs.createEntity();
	ecs.addComponent<TransformComponent>(crosshairEnt);
	SpriteComponent& crosshairSprite = ecs.addComponent<SpriteComponent>(crosshairEnt);
	crosshairSprite.setAnchor(DirectX::XMFLOAT2({ 0.5f, 0.5f }));
	crosshairSprite.setSprite(g_res.crosshair);
	crosshairSprite.setScale(DirectX::XMFLOAT2({ 1, 1 }));
	crosshairSprite.setPosition(DirectX::XMFLOAT2(windowSize.x / 2.f, windowSize.y / 2.f));
	crosshairSprite.setLayer(1);
	
	ECS::Entity healthOutlineEnt = ecs.createEntity();
	ecs.addComponent<TransformComponent>(healthOutlineEnt);
	g_state.healthBarOutline = &ecs.addComponent<SpriteComponent>(healthOutlineEnt);
	g_state.healthBarOutline->setAnchor(DirectX::XMFLOAT2({ 0, 0.5f }));
	g_state.healthBarOutline->setSprite(g_res.healthbarOutline);
	g_state.healthBarOutline->setScale(DirectX::XMFLOAT2({ 0.5f, 0.5f }));
	g_state.healthBarOutline->setLayer(1);

	ECS::Entity healthEnt = ecs.createEntity();
	ecs.addComponent<TransformComponent>(healthEnt);
	g_state.healthBar = &ecs.addComponent<SpriteComponent>(healthEnt);
	g_state.healthBar->setAnchor(DirectX::XMFLOAT2({ 0, 0.5f }));
	g_state.healthBar->setSprite(g_res.healthbar);
	g_state.healthBar->setScale(DirectX::XMFLOAT2({ 0.5f, 0.5f }));
	g_state.healthBar->setLayer(2);

	ECS::Entity vehicleHealthOutlineEnt = ecs.createEntity();
	ecs.addComponent<TransformComponent>(vehicleHealthOutlineEnt);
	g_state.vehicleHealthBarOutline = &ecs.addComponent<SpriteComponent>(vehicleHealthOutlineEnt);
	g_state.vehicleHealthBarOutline->setAnchor(DirectX::XMFLOAT2({ 0, 0.5f }));
	g_state.vehicleHealthBarOutline->setSprite(g_res.vehicleHealthbarOutline);
	g_state.vehicleHealthBarOutline->setScale(DirectX::XMFLOAT2({ 0.5f, 0.5f }));
	g_state.vehicleHealthBarOutline->setLayer(1);

	ECS::Entity vehicleHealthEnt = ecs.createEntity();
	ecs.addComponent<TransformComponent>(vehicleHealthEnt);
	g_state.vehicleHealthBar = &ecs.addComponent<SpriteComponent>(vehicleHealthEnt);
	g_state.vehicleHealthBar->setAnchor(DirectX::XMFLOAT2({ 0, 0.5f }));
	g_state.vehicleHealthBar->setSprite(g_res.vehicleHealthbar);
	g_state.vehicleHealthBar->setScale(DirectX::XMFLOAT2({ 0.5f, 0.5f}));
	g_state.vehicleHealthBar->setLayer(2);

	g_state.healthBarOutline->setPosition(DirectX::XMFLOAT2({ windowSize.x * 0.02f, windowSize.y * 0.95f }));
	g_state.healthBar->setPosition(DirectX::XMFLOAT2({ windowSize.x * 0.02f, windowSize.y * 0.95f }));
	g_state.vehicleHealthBarOutline->setPosition(DirectX::XMFLOAT2({ windowSize.x * 0.02f, windowSize.y * 0.92f }));
	g_state.vehicleHealthBar->setPosition(DirectX::XMFLOAT2({ windowSize.x * 0.02f, windowSize.y * 0.92f }));

	ECS::Entity overScreenEnt = ecs.createEntity();
	ecs.addComponent<TransformComponent>(overScreenEnt);
	g_state.gameOverScreen = &ecs.addComponent<SpriteComponent>(overScreenEnt);
	g_state.gameOverScreen->setAnchor(DirectX::XMFLOAT2({ 0.0f, 0.0f }));
	g_state.gameOverScreen->setSprite(ResourceManager::getSprite("tex-gameover", 1, 1));
	g_state.gameOverScreen->setScale(DirectX::XMFLOAT2({ 0, 0 }));
	g_state.gameOverScreen->setLayer(6);
}

void gameManagerUpdateFunction() {
	ECS& ecs = APPLICATION.getECS();
	float delta = APPLICATION.deltaTime();
	RailFollower& follower = ecs.getComponent<RailFollower>(g_state.vehicle);
	PlayerComponent& player = ecs.getComponent<PlayerComponent>(g_state.player);

	g_state.metersTravelled += follower.speed * delta;
	std::string score = "Distance Travelled: ";
	score += std::to_string((int)g_state.metersTravelled);
	score += "m";
	g_state.scoreText->setString(score);
	g_state.healthBar->setScale(DirectX::XMFLOAT2({ (float)player.health / (float)player.maxHealth * 0.5f, 1 * 0.5f }));
	g_state.vehicleHealthBar->setScale(DirectX::XMFLOAT2({ (float)player.vehicleHealth / (float)player.maxVehicleHealth * 0.5f, 1 * 0.5f }));
	
	std::string bullets = std::to_string(player.bulletCount);
	bullets += "/";
	bullets += std::to_string(player.maxBulletCount);
	g_state.bulletText->setString(bullets);

	if (player.health <= 0) {
		// LOSE SCREEN

		std::string score = "You have travelled: ";
		score += std::to_string((int)g_state.metersTravelled);
		score += " meters!";

		g_state.healthBar->setScale({ 0, 0 });
		g_state.scoreText->setScale(0);
		g_state.healthBar->setScale({ 0, 0 });
		g_state.healthBarOutline->setScale({ 0, 0 });
		g_state.vehicleHealthBar->setScale({ 0, 0 });
		g_state.vehicleHealthBarOutline->setScale({ 0, 0 });

		g_state.gameOverScoreText->setString(score);
		g_state.gameOverScoreText->setScale(0.5f);
		g_state.restartText->setScale(0.35f);
		g_state.gameOverScreen->setScale({ static_cast<float>(1920) / static_cast<float>(g_state.gameOverScreen->getSprite()->spriteSize.x), static_cast<float>(1080) / static_cast<float>(g_state.gameOverScreen->getSprite()->spriteSize.y) });

		APPLICATION.setTimescale(0);

		if (InputManager::getActionDown("restart")) {
			ecs.reset();
		}
	}
}
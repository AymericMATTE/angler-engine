#pragma once

#include "engine/ecs/system.h"
#include "engine/application.h"

void setupGameInputsFunction();

void resetGameState();

SCRIPT_BEGIN(SetupGameInputs, Script::Lifetime::START)
{
	setupGameInputsFunction();
}
SCRIPT_END()

void gameStartupFunction();

SCRIPT_BEGIN(GameStartup, Script::Lifetime::START)
{
	gameStartupFunction();
}
SCRIPT_END()

void gameManagerStartFunction();

SCRIPT_BEGIN(GameManagerStart, Script::START)
{
	gameManagerStartFunction();
}
SCRIPT_END()

void gameManagerUpdateFunction();

SCRIPT_BEGIN(GameManagerUpdate, Script::UPDATE)
{
	gameManagerUpdateFunction();
}
SCRIPT_END()
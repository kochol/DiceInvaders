#include <windows.h>
#include "engine/Engine.h"
#include "game/Player.h"
#include "game/Alien.h"
#include "game/Bomb.h"
#include "game/Rocket.h"
#include "game/Game.h"

int APIENTRY WinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	LPSTR commandLine,
	int commandShow)
{
	Engine::Config config;
	config.screen_width = 510;
	config.screen_height = 500;

	Engine::Init(config);
	Engine::InitLayer(Engine::LAYER_ID_PLAYER, 1);
	Engine::InitLayer(Engine::LAYER_ID_ALIEN, 100);
	Engine::InitLayer(Engine::LAYER_ID_BOMB, 100);
	Engine::InitLayer(Engine::LAYER_ID_ROCKET, 100);

	Engine::RegisterComponentType(Engine::COMPONENT_TYPE_MODEL,
	{
		{ Engine::CALLBACK_STAGE_INIT, Engine::InitModels },
		{ Engine::CALLBACK_STAGE_PHYSICS_BROAD, Engine::UpdatePhysicsBroadPhase },
		{ Engine::CALLBACK_STAGE_PHYSICS_NARROW, Engine::UpdatePhysicsNarrowPhase },
		{ Engine::CALLBACK_STAGE_RENDER, Engine::Render },
	});

	Engine::RegisterComponentType(Engine::COMPONENT_TYPE_PLAYER,
	{
		{ Engine::CALLBACK_STAGE_INIT , Game::InitPlayerManager },
		{ Engine::CALLBACK_STAGE_UPDATE , Game::UpdatePlayerFromInput },
		{ Engine::CALLBACK_STAGE_SHUTDOWN , Game::ShutdownPlayerManager }
	});

	Engine::RegisterComponentType(Engine::COMPONENT_TYPE_ALIEN,
	{
		{ Engine::CALLBACK_STAGE_INIT , Game::InitAlienManager },
		{ Engine::CALLBACK_STAGE_PRE_UPDATE , Game::HandleAlienCollisions },
		{ Engine::CALLBACK_STAGE_UPDATE , Game::UpdateAliens },
		{ Engine::CALLBACK_STAGE_POST_UPDATE , Game::SpawnAliens },
		{ Engine::CALLBACK_STAGE_SHUTDOWN , Game::ShutdownAlienManager }
	});

	Engine::RegisterComponentType(Engine::COMPONENT_TYPE_BOMB,
	{
		{ Engine::CALLBACK_STAGE_INIT , Game::InitBombManager },
		{ Engine::CALLBACK_STAGE_PRE_UPDATE , Game::HandleBombCollisions },
		{ Engine::CALLBACK_STAGE_UPDATE , Game::UpdateBombs },
		{ Engine::CALLBACK_STAGE_SHUTDOWN , Game::ShutdownBombManager }
	});

	Engine::RegisterComponentType(Engine::COMPONENT_TYPE_ROCKET,
	{
		{ Engine::CALLBACK_STAGE_INIT , Game::InitRocketManager },
		{ Engine::CALLBACK_STAGE_PRE_UPDATE , Game::HandleRocketCollisions },
		{ Engine::CALLBACK_STAGE_UPDATE , Game::UpdateRockets },
		{ Engine::CALLBACK_STAGE_SHUTDOWN , Game::ShutdownRocketManager }
	});

	Engine::g_context->world->collisionMasks.push_back({ Engine::LAYER_ID_PLAYER, Engine::LAYER_ID_ALIEN });
	Engine::g_context->world->collisionMasks.push_back({ Engine::LAYER_ID_PLAYER, Engine::LAYER_ID_BOMB });
	Engine::g_context->world->collisionMasks.push_back({ Engine::LAYER_ID_ALIEN, Engine::LAYER_ID_ROCKET });

	Engine::InitComponents();

	Game::g_currentSession.state = Game::GAME_STATE_PRE_GAME;

	while (Engine::ShouldRun())
	{
		Engine::Update();
		Game::Update();
	}

	Engine::Shutdown();

	return 0;
}




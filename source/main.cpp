#include <windows.h>

#include "engine/engine.h"
#include "game/player.h"
#include "game/alien.h"
#include "game/bomb.h"
#include "game/rocket.h"
#include "game/game.h"

int APIENTRY WinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	LPSTR commandLine,
	int commandShow)
{
	// Initialize the engine and window
	Engine::Config config;
	config.screen_width = 510;
	config.screen_height = 500;
	Engine::Init(config);

	// Initialize layers
	Engine::InitLayer(Engine::LAYER_ID_PLAYER, 1);
	Engine::InitLayer(Engine::LAYER_ID_ALIEN, 100);
	Engine::InitLayer(Engine::LAYER_ID_BOMB, 100);
	Engine::InitLayer(Engine::LAYER_ID_ROCKET, 100);

	// Register component types
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

	// Define collision masks
	Engine::AddCollisionMask(Engine::LAYER_ID_PLAYER, Engine::LAYER_ID_ALIEN);
	Engine::AddCollisionMask(Engine::LAYER_ID_PLAYER, Engine::LAYER_ID_BOMB);
	Engine::AddCollisionMask(Engine::LAYER_ID_ALIEN, Engine::LAYER_ID_ROCKET);

	// Initilize the previously registered component managers
	Engine::InitComponents();

	// Eneter the main loop
	while (Engine::ShouldRun())
	{
		Engine::Update();
		Game::Update();
	}


	// Shutdown the engine and free allocated resources
	Engine::Shutdown();

	// Actually the majority of the allocated memory 
	// is going to be freed here (~RosterPool). This free-
	// form memory allocation / deallocation style is used for
	// the sake of simplicity, for actual usages, custom 
	// global memory allocators should be used instead.
	return 0;
}




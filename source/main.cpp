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
	config.screen_width = 500;
	config.screen_height = 500;

	Engine::Init(config);
	Engine::AddLayer(Engine::LayerId::PLAYER, 1);
	Engine::AddLayer(Engine::LayerId::ALIEN, 100);
	Engine::AddLayer(Engine::LayerId::BOMB, 100);
	Engine::AddLayer(Engine::LayerId::ROCKET, 100);

	Engine::RegisterComponentType(Engine::ComponentType::PLAYER,
	{
		{ Engine::CallbackStage::INIT , Game::InitPlayerManager },
		{ Engine::CallbackStage::UPDATE , Game::UpdatePlayerFromInput },
		{ Engine::CallbackStage::SHUTDOWN , Game::ShutdownPlayerManager }
	});

	Engine::RegisterComponentType(Engine::ComponentType::ALIEN,
	{
		{ Engine::CallbackStage::INIT , Game::InitAlienManager },
		{ Engine::CallbackStage::PRE_UPDATE , Game::HandleAlienCollisions },
		{ Engine::CallbackStage::UPDATE , Game::UpdateAliens },
		{ Engine::CallbackStage::POST_UPDATE , Game::SpawnAliens },
		{ Engine::CallbackStage::SHUTDOWN , Game::ShutdownAlienManager }
	});

	Engine::RegisterComponentType(Engine::ComponentType::BOMB,
	{
		{ Engine::CallbackStage::INIT , Game::InitBombManager },
		{ Engine::CallbackStage::PRE_UPDATE , Game::HandleBombCollisions },
		{ Engine::CallbackStage::UPDATE , Game::UpdateBombs },
		{ Engine::CallbackStage::SHUTDOWN , Game::ShutdownBombManager }
	});

	Engine::RegisterComponentType(Engine::ComponentType::ROCKET,
	{
		{ Engine::CallbackStage::INIT , Game::InitRocketManager },
		{ Engine::CallbackStage::PRE_UPDATE , Game::HandleRocketCollisions },
		{ Engine::CallbackStage::UPDATE , Game::UpdateRockets },
		{ Engine::CallbackStage::SHUTDOWN , Game::ShutdownRocketManager }
	});

	Engine::g_context->world->collisionMasks.push_back({ Engine::LayerId::PLAYER, Engine::LayerId::ALIEN });
	Engine::g_context->world->collisionMasks.push_back({ Engine::LayerId::PLAYER, Engine::LayerId::BOMB });
	Engine::g_context->world->collisionMasks.push_back({ Engine::LayerId::ALIEN, Engine::LayerId::ROCKET });

	Engine::InitComponents();

	Game::SpawnPlayer();

	while (Engine::ShouldRun())
	{
		Engine::PreUpdate();

		Engine::RegUpdate();

		Engine::Render();

		Engine::PostUpdate();
	}

	Engine::Shutdown();

	return 0;
}




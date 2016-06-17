#include <windows.h>
#include "engine/Engine.h"
#include "game/Player.h"
#include "game/Alien.h"

int APIENTRY WinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	LPSTR commandLine,
	int commandShow)
{
	Engine::Config config;
	config.screen_width = 800;
	config.screen_height = 600;

	Engine::Init(config);
	Engine::AddLayer(Engine::LayerId::PLAYER, 1);
	Engine::AddLayer(Engine::LayerId::ALIEN, 100);
	Engine::AddLayer(Engine::LayerId::BOMB, 100);
	Engine::AddLayer(Engine::LayerId::ROCKET, 100);

	Engine::RegisterComponentType(Engine::ComponentType::PLAYER,
	{
		{ Engine::CallbackStage::INIT , Game::InitPlayerManager },
		{ Engine::CallbackStage::REG_UPDATE , Game::UpdatePlayerFromInput },
		{ Engine::CallbackStage::SHUTDOWN , Game::ShutdownPlayer }
	});

	Engine::RegisterComponentType(Engine::ComponentType::ALIEN,
	{
		{ Engine::CallbackStage::INIT , Game::InitAlienManager },
		{ Engine::CallbackStage::PRE_UPDATE , Game::HandleAlienCollisions },
		{ Engine::CallbackStage::REG_UPDATE , Game::UpdateAliens },
		{ Engine::CallbackStage::POST_UPDATE , Game::SpawnAliens },
		{ Engine::CallbackStage::SHUTDOWN , Game::ShutdownAliens }
	});

	Engine::g_context->world->collisionMasks.push_back({ Engine::LayerId::PLAYER, Engine::LayerId::ALIEN });
	Engine::g_context->world->collisionMasks.push_back({ Engine::LayerId::PLAYER, Engine::LayerId::BOMB });
	Engine::g_context->world->collisionMasks.push_back({ Engine::LayerId::ALIEN, Engine::LayerId::ROCKET });

	Engine::InitComponents();

	Engine::EntityHandle player = Game::SpawnPlayer();

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




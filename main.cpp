#include <windows.h>
#include "Engine.h"
#include "Player.h"

int APIENTRY WinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	LPSTR commandLine,
	int commandShow)
{
	Engine::Config config;
	config.screen_width = 640;
	config.screen_height = 480;

	Engine::Init(config);
	Engine::AddLayer(Engine::LayerId::PLAYER, 1);
	Engine::AddLayer(Engine::LayerId::ALIEN, 100);
	Engine::AddLayer(Engine::LayerId::BOMB, 100);
	Engine::AddLayer(Engine::LayerId::ROCKET, 100);

	Engine::ComponentManager::Callbacks callbacks;
	callbacks.initCallback = Game::InitPlayerManager;
	callbacks.updateCallbacks[Engine::UpdateStage::REG_UPDATE] = Game::UpdatePlayerFromInput;
	Engine::RegisterComponentType(Engine::ComponentType::PLAYER, callbacks);

	Engine::EntityHandle player = Game::CreatePlayer(Engine::g_context->world->components[Engine::ComponentType::PLAYER]);

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




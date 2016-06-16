#include <windows.h>
#include "Engine.h"
#include "Player.h"

int APIENTRY WinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	LPSTR commandLine,
	int commandShow)
{
	Engine::Context *ctx = Engine::Init(640, 480);

	Engine::EntityHandle player = Game::CreatePlayer(ctx);

	float lastTime = ctx->system->getElapsedTime();
	bool running = true;
	while (running)
	{
		const float newTime = ctx->system->getElapsedTime();
		const float dt = newTime - lastTime;
		lastTime = newTime;
		
		Engine::KeyStatus keys;
		ctx->system->getKeyStatus(keys);

		Game::UpdatePlayerFromInput(dt, player, ctx->world, keys);

		Engine::DrawModels(*ctx->world);

		running = ctx->system->update();
	}

	Engine::Shutdown(ctx);

	return 0;
}




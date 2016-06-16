#include <windows.h>
#include "Engine.h"

int APIENTRY WinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	LPSTR commandLine,
	int commandShow)
{
	Engine::Context *ctx = Engine::Init(640, 480);

	std::string playerSpriteName = "data/player.bmp";
	Engine::ResourceHandle playerSpriteHandle = Engine::LoadSprite(ctx->system, ctx->resources, playerSpriteName);
	Engine::Sprite playerSprite = Engine::LookupSprite(*ctx->resources, playerSpriteHandle);
	
	Engine::EntityHandle player = ctx->world->handleManager.Create();
	Engine::ComponentHandle playerModel = Engine::CreateModel(ctx->world, player, playerSprite);

	ctx->world->transforms[playerModel.index].y = 480 - 32;
	ctx->world->transforms[playerModel.index].x = 320;

	float lastTime = ctx->system->getElapsedTime();
	while (ctx->system->update())
	{
		float newTime = ctx->system->getElapsedTime();
		float move = (newTime - lastTime) * 160.0f;
		lastTime = newTime;
		
		Engine::IDiceInvaders::KeyStatus keys;
		ctx->system->getKeyStatus(keys);
		if (keys.right)
			ctx->world->transforms[playerModel.index].x += move;
		else if (keys.left)
			ctx->world->transforms[playerModel.index].x -= move;

		Engine::DrawModels(*ctx->world);
	}

	Engine::Shutdown(ctx);

	return 0;
}




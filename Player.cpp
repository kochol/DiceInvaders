#include "Player.h"

namespace Game
{
	void UpdatePlayerFromInput(const float dt, const Engine::EntityHandle playerEntity, Engine::World *const world, const Engine::KeyStatus& keys)
	{
		const float move = dt * 160.0f;

		const Engine::ComponentHandle playerModel = world->modelMap[playerEntity];

		if (keys.right)
			world->transforms[playerModel.index].x += move;
		else if (keys.left)
			world->transforms[playerModel.index].x -= move;
	}

	Engine::EntityHandle CreatePlayer(Engine::Context *const context)
	{
		const std::string playerSpriteName = "data/player.bmp";
		const Engine::ResourceHandle playerSpriteHandle = Engine::LoadSprite(context->system, context->resources, playerSpriteName);
		const Engine::Sprite playerSprite = Engine::LookupSprite(*context->resources, playerSpriteHandle);

		const Engine::EntityHandle player = context->world->handleManager.Create();
		const Engine::ComponentHandle playerModel = Engine::CreateModel(context->world, player, playerSprite);

		context->world->transforms[playerModel.index].y = context->height - 32;
		context->world->transforms[playerModel.index].x = context->width / 2;

		return player;
	}
}
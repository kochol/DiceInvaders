#include "Player.h"

namespace Game
{
	void InitPlayerManager(Engine::ComponentManager* const manager)
	{
		manager->components.Init(1, { sizeof(Player) });
	}

	void UpdatePlayerFromInput(Engine::ComponentManager* const manager)
	{
		const Engine::FrameData *const frame_data = Engine::g_context->frame_data;

		Player *const playerCompData = manager->components.Resolve<Player>(0);

		const float move = frame_data->dt * 160.0f;

		Engine::Transform *const transform = Engine::ResolveTransform(playerCompData->model);

		if (frame_data->keys.right)
			transform->x += move;
		else if (frame_data->keys.left)
			transform->x -= move;
	}

	Engine::EntityHandle CreatePlayer()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::ComponentType::PLAYER);
		const Engine::EntityHandle player = Engine::CreateEntity(Engine::LayerId::PLAYER);
		const std::string sprite_name = "data/player.bmp";
		const Engine::ResourceHandle sprite = Engine::LoadSprite(sprite_name);
		const Engine::ComponentHandle model = Engine::CreateModel(player, sprite);

		Engine::ComponentHandle playerComp;
		playerComp.index = manager->components.Alloc();
		playerComp.header.layer = player.header.layer;
		playerComp.header.type = manager->type;

		Player *const playerCompData = manager->components.Resolve<Player>(0);
		playerCompData->entity = player;
		playerCompData->model = model;
		playerCompData->health = 3;
		playerCompData->lastFired = 0.f;
		
		Engine::Transform *const transform = Engine::ResolveTransform(model);

		transform->x = Engine::g_context->config->screen_width / 2;
		transform->y = Engine::g_context->config->screen_height - 32;

		return player;
	}
}
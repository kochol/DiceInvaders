#include "Player.h"

namespace Game
{
	void UpdatePlayerFromInput(const Engine::EntityHandle player)
	{
		const Engine::FrameData *const frame_data = Engine::g_context->frame_data;
		const float move = frame_data->dt * 160.0f;

		const Engine::ComponentHandle model = Engine::LookupModel(player);
		Engine::Transform *const transform = Engine::ResolveTransform(model);

		if (frame_data->keys.right)
			transform->x += move;
		else if (frame_data->keys.left)
			transform->x -= move;
	}

	Engine::EntityHandle CreatePlayer()
	{
		const std::string sprite_name = "data/player.bmp";
		const Engine::ResourceHandle sprite = Engine::LoadSprite(sprite_name);

		const Engine::EntityHandle player = Engine::CreateEntity(Engine::LayerId::PLAYER);
		const Engine::ComponentHandle model = Engine::CreateModel(player, sprite);

		Engine::Transform *const transform = Engine::ResolveTransform(model);

		transform->y = Engine::g_context->config->screen_height - 32;
		transform->x = Engine::g_context->config->screen_width / 2;

		return player;
	}
}
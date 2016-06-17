#include "Player.h"

namespace Game
{
	struct _PlayerManager
	{
		Engine::ResourceHandle sprite;
	};

	void InitPlayerManager(Engine::ComponentManager* const manager)
	{
		manager->components.Init(1, { sizeof(Player) });

		_PlayerManager *const data = new _PlayerManager;

		data->sprite = Engine::LoadSprite("data/player.bmp");

		manager->customData = data;
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

	Engine::EntityHandle SpawnPlayer()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::ComponentType::PLAYER);
		const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LayerId::PLAYER);
		
		Engine::ResourceHandle sprite = reinterpret_cast<_PlayerManager*>(manager->customData)->sprite;
		const Engine::ComponentHandle model = Engine::CreateModel(entity, sprite);

		Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::ComponentType::PLAYER);
		
		Player *const componentData = manager->components.Resolve<Player>(component.index);
		componentData->model = model;
		componentData->health = 3;
		componentData->lastFired = 0.f;
		
		Engine::Transform *const transform = Engine::ResolveTransform(model);

		transform->x = Engine::g_context->config->screen_width / 2;
		transform->y = Engine::g_context->config->screen_height - 32;

		return entity;
	}

	void ShutdownPlayer(Engine::ComponentManager* const manager)
	{
		delete manager->customData;
	}
}
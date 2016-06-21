#include "Player.h"
#include "Game.h"

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

		data->sprite = Engine::LoadSprite("player.bmp");

		manager->customData = data;
	}

	void UpdatePlayerFromInput(Engine::ComponentManager* const manager)
	{
		Player *const playerCompData = manager->components.Resolve<Player>(0);

		const float move = Engine::DeltaTime() * 300.0f;

		Engine::Transform *const transform = Engine::ResolveModelTransform(playerCompData->model);

		if (Engine::Keys().right)
			transform->position.x += move;
		else if (Engine::Keys().left)
			transform->position.x -= move;

		const float max_x = Engine::ScreenWidth() - 35.f;
		const float min_x = 3.f;
		transform->position.x = max(min_x, min(max_x, transform->position.x));

		if (Engine::Keys().fire && Engine::Time() - playerCompData->lastFired > 0.25f)
		{
			playerCompData->lastFired = Engine::Time();
			SpawnRocket(transform->position.x, transform->position.y - 35);
		}


		char playerHealth[100];
		sprintf_s(playerHealth, "player health: %d", playerCompData->health);
		Engine::g_context->system->drawText(0, 40, playerHealth);
	}

	Engine::EntityHandle SpawnPlayer()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_PLAYER);
		const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LAYER_ID_PLAYER);
		
		Engine::ResourceHandle sprite = reinterpret_cast<_PlayerManager*>(manager->customData)->sprite;
		const Engine::ComponentHandle model = Engine::CreateModel(entity, sprite);

		Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_PLAYER, model);
		
		Player *const componentData = manager->components.Resolve<Player>(component.index);
		componentData->health = 3;
		componentData->lastFired = 0.f;
		
		*Engine::ResolveModelTransform(model) = 
		{ {
			Engine::g_context->config->screen_width / 2.f,
			Engine::g_context->config->screen_height - 32.f
		} };

		Engine::Collider *const collider = Engine::ResolveModelCollider(model);
		collider->localBb.center = { 16.f, 15.5f };
		collider->localBb.halfSize = { 14.f, 8.5f };

		g_currentSession.player = component;

		return entity;
	}

	void DestroyPlayer()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_PLAYER);

		uint16_t count = manager->components.Size();

		const Engine::LayerId layer = Engine::LAYER_ID_ALIEN;
		const uint16_t *const indexes = Engine::ResolveModelIndexes(layer);
		Engine::BaseComponent *const model_components = Engine::ResolveModelComponentData(layer);

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];

			const Engine::EntityHandle entity = model_components[index].entity;
			const Engine::ComponentHandle model = model_components[index].model;
			const Engine::ComponentHandle component = manager->componentMap[entity];

			Engine::DestroyModel(model);
			Engine::DestroyEntity(entity);
			manager->components.Free(component.index);
			manager->componentMap.erase(entity);
		}
	}

	void ShutdownPlayerManager(Engine::ComponentManager* const manager)
	{
		delete manager->customData;
	}

	void DamagePlayer()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_PLAYER);
		Player *const componentData = manager->components.Resolve<Player>(g_currentSession.player.index);
		
		componentData->health -= 1;

		if (componentData->health <= 0)
		{
			GameOver();
			//TODO: kill player
		}
	}

	void ScorePlayer()
	{
		g_currentSession.score++;
	}
}
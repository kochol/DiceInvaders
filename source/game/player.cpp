/* ---------------------------------------------------------------------------
**
** player.cpp
** The Player component
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#include "player.h"

#include <algorithm>

#include "game.h"

namespace Game
{
	struct _PlayerManager
	{
		Engine::ResourceHandle sprite;
	};

	void InitPlayerManager(Engine::ComponentManager::LayerData *const data)
	{
		if (data->layer_id != Engine::LAYER_ID_PLAYER)
			return;

		const Engine::World::LayerData *const world_layer_data = Engine::GetLayerData(data->layer_id);

		data->components.Init(world_layer_data->max_entities, { sizeof(Engine::BaseComponent), sizeof(Player) });

		_PlayerManager *const custom_data = new _PlayerManager;

		custom_data->sprite = Engine::LoadSprite("player.bmp");

		data->custom_data = custom_data;
	}

	void UpdatePlayerFromInput(Engine::ComponentManager::LayerData *const data)
	{
		if (data->layer_id != Engine::LAYER_ID_PLAYER ||
			State() != GAME_STATE_IN_GAME)
			return;

		Engine::BaseComponent *const component = data->components.Resolve<Engine::BaseComponent>(0, COMPONENT_DATA_BASE);
		Player *const player = data->components.Resolve<Player>(0, COMPONENT_DATA_CUSTOM);

		const float move = Engine::DeltaTime() * 300.0f;

		Engine::Transform *const transform = Engine::ResolveComponentSegment<Engine::Transform>(component->model, Engine::MODEL_DATA_TRANSFORM);

		if (Engine::Keys().right)
			transform->position.x += move;
		else if (Engine::Keys().left)
			transform->position.x -= move;

		const float max_x = Engine::ScreenWidth() - 35.f;
		const float min_x = 3.f;
		transform->position.x = std::max(min_x, std::min(max_x, transform->position.x));

		if (Engine::Keys().fire && Engine::Time() - player->lastFired > 0.25f)
		{
			player->lastFired = Engine::Time();
			SpawnRocket(transform->position.x, transform->position.y - 35);
		}
	}

	void ShutdownPlayerManager(Engine::ComponentManager::LayerData *const data)
	{
		if (data->layer_id != Engine::LAYER_ID_PLAYER)
			return;

		delete data->custom_data;
	}

	Engine::EntityHandle SpawnPlayer()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_PLAYER);
		Engine::ComponentManager::LayerData *const layer = &manager->layer_data[Engine::LAYER_ID_PLAYER];

		const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LAYER_ID_PLAYER);
		const Engine::ComponentHandle model = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_MODEL);
		const Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_PLAYER);

		Engine::ResourceHandle sprite = reinterpret_cast<_PlayerManager*>(layer->custom_data)->sprite;
		*Engine::ResolveComponentSegment<Engine::ResourceHandle>(model, Engine::MODEL_DATA_SPRITE) = sprite;

		Player *const player = layer->components.Resolve<Player>(component.index, COMPONENT_DATA_CUSTOM);
		player->health = 3;
		player->lastFired = 0.f;
		
		Engine::ResolveComponentSegment<Engine::Transform>(model, Engine::MODEL_DATA_TRANSFORM)->position = 
		{
			Engine::g_context->config->screen_width / 2.f,
			Engine::g_context->config->screen_height - 32.f
		};

		Engine::Collider *const collider = Engine::ResolveComponentSegment<Engine::Collider>(model, Engine::MODEL_DATA_COLLIDER);
		collider->local_aabb.center = { 16.f, 15.5f };
		collider->local_aabb.half_size = { 14.f, 8.5f };

		g_currentSession.player = component;

		return entity;
	}

	void DestroyPlayer()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_PLAYER);
		Engine::ComponentManager::LayerData *const layer = &manager->layer_data[Engine::LAYER_ID_PLAYER];

		uint16_t count = layer->components.Size();

		const uint16_t *const indexes = layer->components.Indexes();
		Engine::BaseComponent *const components = layer->components.Data<Engine::BaseComponent>(COMPONENT_DATA_BASE);

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const Engine::BaseComponent *const component = components + index;

			Engine::DestroyEntity(component->entity);
			Engine::DestroyComponent(component->model);
			Engine::DestroyComponent(component->self);
		}
	}

	void DamagePlayer()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_PLAYER);
		Engine::ComponentManager::LayerData *const layer = &manager->layer_data[Engine::LAYER_ID_PLAYER];
		Player *const player = layer->components.Resolve<Player>(g_currentSession.player.index, COMPONENT_DATA_CUSTOM);
		
		player->health -= 1;

		if (player->health <= 0)
		{
			End();
			//TODO: kill player
		}
	}
}
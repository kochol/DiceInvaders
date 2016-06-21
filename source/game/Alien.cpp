#include "Alien.h"
#include <algorithm>
#include <cassert>
#include "Game.h"

namespace Game
{
	struct _AlienManager
	{
		Engine::ResourceHandle sprite[2];
		float lastMoved;
		int moveDirection;
		bool shouldChangeDirection;
	};

	void InitAlienManager(Engine::ComponentManager::LayerData *const data)
	{
		if (data->layerId != Engine::LAYER_ID_ALIEN)
			return;
		const Engine::World::LayerData *const world_layer_data = Engine::GetLayerData(data->layerId);

		data->components.Init(world_layer_data->maxEntities, { sizeof(Engine::BaseComponent), sizeof(Alien) });

		_AlienManager *const custom_data = new _AlienManager();

		custom_data->sprite[0] = Engine::LoadSprite("enemy1.bmp");
		custom_data->sprite[1] = Engine::LoadSprite("enemy2.bmp");
		custom_data->lastMoved = 0.f;
		custom_data->moveDirection = 1;
		custom_data->shouldChangeDirection = false;

		data->customData = custom_data;
	}

	void HandleAlienCollisions(Engine::ComponentManager::LayerData *const data)
	{
		uint16_t count = data->components.Size();
		if (count == 0)
			return;

		const uint16_t *const indexes = data->components.Indexes();
		const Engine::BaseComponent *const components = data->components.Data<Engine::BaseComponent>(COMPONENT_DATA_BASE);

		Engine::Collision *const model_collisions = Engine::ResolveComponentSegmentData<Engine::Collision>(
			Engine::COMPONENT_TYPE_MODEL, data->layerId, Engine::MODEL_DATA_COLLISION);

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const Engine::BaseComponent* component = components + index;

			const Engine::Collision *const model_collision = model_collisions + component->model.index;

			if (model_collision->collidedLayers ||
				model_collision->boundary)
			{
				if (model_collision->collidedLayers & (1 << Engine::LAYER_ID_PLAYER))
					DamagePlayer();

				Engine::DestroyEntity(component->entity);
				Engine::DestroyComponent(component->model);
				Engine::DestroyComponent(component->self);
			}
		}
	}

	void UpdateAliens(Engine::ComponentManager::LayerData *const data)
	{
		const uint16_t count = data->components.Size();
		if (count == 0)
			return;

		_AlienManager *const custom_data = reinterpret_cast<_AlienManager*>(data->customData);

		const float delay = 0.1f + (data->components.Size() / 50.f);

		if (Engine::Time() - custom_data->lastMoved < delay)
			return;

		custom_data->lastMoved = Engine::Time();

		const float bombDropChance = 1.5f / static_cast<float>(count + 10);

		const Engine::BoundingBox layer_bb = Engine::GetLayerData(data->layerId)->boundingBox;

		const float min_x = layer_bb.center.x - layer_bb.halfSize.x;
		const float max_x = layer_bb.center.x + layer_bb.halfSize.x;
		custom_data->shouldChangeDirection =
			(custom_data->moveDirection < 0 && min_x < 16.f) ||
			(custom_data->moveDirection > 0 && max_x > Engine::ScreenWidth() - 16.f);

		if (custom_data->shouldChangeDirection)
			custom_data->moveDirection = -custom_data->moveDirection;

		const float move_x = custom_data->shouldChangeDirection ? 0.f : 32.f * custom_data->moveDirection;
		const float move_y = custom_data->shouldChangeDirection ? 32.f : 0.f;

		const uint16_t *const indexes = data->components.Indexes();
		Engine::BaseComponent *const components = data->components.Data<Engine::BaseComponent>(COMPONENT_DATA_BASE);
		Alien *const aliens = data->components.Data<Alien>(COMPONENT_DATA_CUSTOM);

		Engine::Transform *const transforms = Engine::ResolveComponentSegmentData<Engine::Transform>(
			Engine::COMPONENT_TYPE_MODEL, data->layerId, Engine::MODEL_DATA_TRANSFORM);

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const uint16_t model_index = components[index].model.index;

			transforms[model_index].position.x += move_x;
			transforms[model_index].position.y += move_y;

			if (Engine::Time() - aliens[index].lastDroped > 1.f &&
				Engine::Random() < bombDropChance)
			{
				aliens[index].lastDroped = Engine::Time();
				SpawnBomb(transforms[model_index].position.x, transforms[model_index].position.y);
			}
		}
	}

	void SpawnAliens(Engine::ComponentManager::LayerData *const data)
	{
		if (data->layerId != Engine::LAYER_ID_ALIEN ||
			data->components.Size() > 0)
			return;

		_AlienManager *const custom_data = reinterpret_cast<_AlienManager*>(data->customData);
		custom_data->moveDirection = 1;
		custom_data->shouldChangeDirection = false;
		custom_data->lastMoved = Engine::FrameData().time;

		Alien *const aliens = data->components.Data<Alien>(COMPONENT_DATA_CUSTOM);

		Engine::Transform *const transforms = Engine::ResolveComponentSegmentData<Engine::Transform>(
			Engine::COMPONENT_TYPE_MODEL, data->layerId, Engine::MODEL_DATA_TRANSFORM);
		Engine::Collider *const colliders = Engine::ResolveComponentSegmentData<Engine::Collider>(
			Engine::COMPONENT_TYPE_MODEL, data->layerId, Engine::MODEL_DATA_COLLIDER);
		Engine::ResourceHandle *const sprites = Engine::ResolveComponentSegmentData<Engine::ResourceHandle>(
			Engine::COMPONENT_TYPE_MODEL, data->layerId, Engine::MODEL_DATA_SPRITE);

		for (int16_t x = 0; x < 11; x++)
		{
			for (int16_t y = 0; y < 5; y++)
			{
				const Engine::EntityHandle entity = Engine::CreateEntity(data->layerId);
				const Engine::ComponentHandle model = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_MODEL);
				const Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_ALIEN);

				aliens[component.index].lastDroped = 0.f;

				transforms[model.index].position = { x * 35.f, y * 35.f + 100.f };
				colliders[model.index].localBb.center = { 16.f, 16.f };
				colliders[model.index].localBb.halfSize = { 15.f, 11.f };
				
				const Engine::ResourceHandle sprite = custom_data->sprite[y < 2];
				sprites[model.index] = sprite;
			}
		}
	}

	void ShutdownAlienManager(Engine::ComponentManager::LayerData *const data)
	{
		if (data->layerId != Engine::LAYER_ID_ALIEN)
			return;

		delete data->customData;
	}

	void DestroyAliens()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_ALIEN);
		Engine::ComponentManager::LayerData *const layer = &manager->layerData[Engine::LAYER_ID_ALIEN];

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
}


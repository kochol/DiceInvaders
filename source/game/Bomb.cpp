#include "Bomb.h"
#include "Game.h"

namespace Game
{
	struct _BombManager
	{
		Engine::ResourceHandle sprite;
	};

	void InitBombManager(Engine::ComponentManager::LayerData *const data)
	{
		if (data->layerId != Engine::LAYER_ID_BOMB)
			return;

		const Engine::World::LayerData *const world_layer_data = Engine::GetLayerData(data->layerId);

		data->components.Init(world_layer_data->maxEntities, { sizeof(Engine::BaseComponent) });

		_BombManager *const custom_data = new _BombManager;

		custom_data->sprite = Engine::LoadSprite("bomb.bmp");

		data->customData = custom_data;
	}

	void HandleBombCollisions(Engine::ComponentManager::LayerData *const data)
	{
		uint16_t count = data->components.Size();
		if (count == 0)
			return;

		const uint16_t *const indexes = data->components.Indexes();
		const Engine::BaseComponent *const components = data->components.Data<Engine::BaseComponent>(COMPONENT_DATA_BASE);

		Engine::Collision *const model_collisions = Engine::ResolveComponentSegmentData<Engine::Collision>(
			Engine::COMPONENT_TYPE_MODEL,
			data->layerId,
			Engine::MODEL_DATA_COLLISION);

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

	void UpdateBombs(Engine::ComponentManager::LayerData *const data)
	{
		const uint16_t count = data->components.Size();
		if (count == 0)
			return;

		const uint16_t *const indexes = data->components.Indexes();
		Engine::BaseComponent *const components = data->components.Data<Engine::BaseComponent>(COMPONENT_DATA_BASE);

		Engine::Transform *const transforms = Engine::ResolveComponentSegmentData<Engine::Transform>(
			Engine::COMPONENT_TYPE_MODEL, data->layerId, Engine::MODEL_DATA_TRANSFORM);

		const float move = Engine::DeltaTime() * 200.f;

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const uint16_t model_index = components[index].model.index;

			transforms[model_index].position.y += move;
		}
	}

	void ShutdownBombManager(Engine::ComponentManager::LayerData *const data)
	{
		if (data->layerId != Engine::LAYER_ID_BOMB)
			return;

		delete data->customData;
	}

	Engine::EntityHandle SpawnBomb(float x, float y)
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_BOMB);
		Engine::ComponentManager::LayerData *const layer = &manager->layerData[Engine::LAYER_ID_BOMB];

		const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LAYER_ID_BOMB);
		const Engine::ComponentHandle model = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_MODEL);
		const Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_BOMB);

		Engine::ResolveComponentSegment<Engine::Transform>(model, Engine::MODEL_DATA_TRANSFORM)->position = { x, y };

		Engine::ResourceHandle sprite = reinterpret_cast<_BombManager*>(layer->customData)->sprite;
		*Engine::ResolveComponentSegment<Engine::ResourceHandle>(model, Engine::MODEL_DATA_SPRITE) = sprite;

		Engine::Collider *const collider = Engine::ResolveComponentSegment<Engine::Collider>(model, Engine::MODEL_DATA_COLLIDER);
		collider->localBb.center = { 16.f, 16.f };
		collider->localBb.halfSize = { 3.f, 8.f };

		return entity;
	}

	void DestroyBombs()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_BOMB);
		Engine::ComponentManager::LayerData *const layer = &manager->layerData[Engine::LAYER_ID_BOMB];

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

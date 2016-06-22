#include "Rocket.h"
#include "Game.h"

namespace Game
{
	struct _RocketManager
	{
		Engine::ResourceHandle sprite;
	};

	void InitRocketManager(Engine::ComponentManager::LayerData *const data)
	{
		if (data->layer_id != Engine::LAYER_ID_ROCKET)
			return;

		const Engine::World::LayerData *const world_layer_data = Engine::GetLayerData(data->layer_id);

		data->components.Init(world_layer_data->max_entities, { sizeof(Engine::BaseComponent) });

		_RocketManager *const custom_data = new _RocketManager;
		custom_data->sprite = Engine::LoadSprite("rocket.bmp");

		data->custom_data = custom_data;
	}

	void HandleRocketCollisions(Engine::ComponentManager::LayerData *const data)
	{
		uint16_t count = data->components.Size();
		if (count == 0)
			return;

		const uint16_t *const indexes = data->components.Indexes();
		const Engine::BaseComponent *const components = data->components.Data<Engine::BaseComponent>(COMPONENT_DATA_BASE);

		Engine::Collision *const model_collisions = Engine::ResolveComponentSegmentData<Engine::Collision>(
			Engine::COMPONENT_TYPE_MODEL, data->layer_id, Engine::MODEL_DATA_COLLISION);

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const Engine::BaseComponent* component = components + index;

			const Engine::Collision *const model_collision = model_collisions + component->model.index;

			if (model_collision->collided_layers ||
				model_collision->boundary)
			{
				if (model_collision->collided_layers & (1 << Engine::LAYER_ID_ALIEN))
					ScorePlayer();

				Engine::DestroyEntity(component->entity);
				Engine::DestroyComponent(component->model);
				Engine::DestroyComponent(component->self);
			}
		}
	}

	void UpdateRockets(Engine::ComponentManager::LayerData *const data)
	{
		const uint16_t count = data->components.Size();
		if (count == 0)
			return;

		const uint16_t *const indexes = data->components.Indexes();
		Engine::BaseComponent *const components = data->components.Data<Engine::BaseComponent>(COMPONENT_DATA_BASE);

		Engine::Transform *const transforms = Engine::ResolveComponentSegmentData<Engine::Transform>(
			Engine::COMPONENT_TYPE_MODEL, data->layer_id, Engine::MODEL_DATA_TRANSFORM);

		const float move = Engine::DeltaTime() * 1000.f;

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const uint16_t model_index = components[index].model.index;

			transforms[model_index].position.y -= move;
		}
	}

	void ShutdownRocketManager(Engine::ComponentManager::LayerData *const data)
	{
		if (data->layer_id != Engine::LAYER_ID_ROCKET)
			return;

		delete data->custom_data;
	}

	Engine::EntityHandle SpawnRocket(float x, float y)
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_ROCKET);
		Engine::ComponentManager::LayerData *const layer = &manager->layer_data[Engine::LAYER_ID_ROCKET];

		const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LAYER_ID_ROCKET);
		const Engine::ComponentHandle model = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_MODEL);
		const Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_ROCKET);

		Engine::ResolveComponentSegment<Engine::Transform>(model, Engine::MODEL_DATA_TRANSFORM)->position = {x, y};

		Engine::ResourceHandle sprite = reinterpret_cast<_RocketManager*>(layer->custom_data)->sprite;
		*Engine::ResolveComponentSegment<Engine::ResourceHandle>(model, Engine::MODEL_DATA_SPRITE) = sprite;

		Engine::Collider *const collider = Engine::ResolveComponentSegment<Engine::Collider>(model, Engine::MODEL_DATA_COLLIDER);
		collider->local_aabb.center = { 16.f, 16.5f };
		collider->local_aabb.half_size = { 2.f, 9.5f };

		return entity;
	}

	void DestroyRockets()
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_ROCKET);
		Engine::ComponentManager::LayerData *const layer = &manager->layer_data[Engine::LAYER_ID_ROCKET];

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
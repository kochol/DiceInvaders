#include "Rocket.h"
#include "Game.h"

namespace Game
{
	struct _RocketManager
	{
		Engine::ResourceHandle sprite;
	};

	void InitRocketManager(Engine::ComponentManager *const manager)
	{
		manager->components.Init(100, { sizeof(Rocket) });

		_RocketManager *const data = new _RocketManager;
		data->sprite = Engine::LoadSprite("rocket.bmp");

		manager->customData = data;
	}

	void HandleRocketCollisions(Engine::ComponentManager* const manager)
	{
		uint16_t count = manager->components.Size();

		Engine::Layer &layer = Engine::ResolveLayer(Engine::LAYER_ID_ROCKET);
		const uint16_t *const indexes = Engine::ResolveModelIndexes(layer);
		Engine::BaseComponent *const model_components = Engine::ResolveModelComponentData(layer);
		Engine::Collision *const collisions = Engine::ResolveModelCollisionData(layer);;

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];

			if (collisions[index].collidedLayer != Engine::LAYER_ID_NONE ||
				collisions[index].out.any)
			{
				if (collisions[index].collidedLayer == Engine::LAYER_ID_ALIEN)
					ScorePlayer();

				const Engine::EntityHandle entity = model_components[index].entity;
				const Engine::ComponentHandle model = model_components[index].model;
				const Engine::ComponentHandle component = manager->componentMap[entity];

				Engine::DestroyModel(model);
				Engine::DestroyEntity(entity);
				manager->components.Free(component.index);
				manager->componentMap.erase(entity);
				--i;
				--count;
			}
		}
	}

	void UpdateRockets(Engine::ComponentManager* const manager)
	{
		const uint16_t count = manager->components.Size();

		const uint16_t *const indexes = manager->components.Indexes();
		Rocket *const rockets = manager->components.Data<Rocket>();

		Engine::Layer &layer = Engine::ResolveLayer(Engine::LAYER_ID_ROCKET);
		Engine::Transform *const transforms = Engine::ResolveModelTransformData(layer);

		const float move = Engine::DeltaTime() * 1000.f;

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const uint16_t modelIndex = rockets[index].model.index;

			transforms[modelIndex].position.y -= move;
		}
	}

	void ShutdownRocketManager(Engine::ComponentManager* const manager)
	{
		delete manager->customData;
	}

	Engine::EntityHandle SpawnRocket(float x, float y)
	{
		Engine::ComponentManager &manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_ROCKET);
		const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LAYER_ID_ROCKET);

		Engine::ResourceHandle sprite = reinterpret_cast<_RocketManager*>(manager.customData)->sprite;
		const Engine::ComponentHandle model = Engine::CreateModel(entity, sprite);

		Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_ROCKET, model);

		Engine::ResolveModelTransform(model) = { {x, y} };

		return entity;
	}
}
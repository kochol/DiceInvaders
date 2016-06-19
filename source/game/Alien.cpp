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

	void InitAlienManager(Engine::ComponentManager *const manager)
	{
		manager->components.Init(100, { sizeof(Alien) });

		_AlienManager *const data = new _AlienManager();
		
		data->sprite[0] = Engine::LoadSprite("enemy1.bmp");
		data->sprite[1] = Engine::LoadSprite("enemy2.bmp");
		data->lastMoved = 0.f;
		data->moveDirection = 1;
		data->shouldChangeDirection = false;

		manager->customData = data;
	}

	void HandleAlienCollisions(Engine::ComponentManager* const manager)
	{
		uint16_t count = manager->components.Size();

		const Engine::LayerId layer = Engine::LAYER_ID_ALIEN;
		const uint16_t *const indexes = Engine::ResolveModelIndexes(layer);
		Engine::BaseComponent *const model_components = Engine::ResolveModelComponentData(layer);
		Engine::Collision *const collisions = Engine::ResolveModelCollisionData(layer);;

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];

			if (collisions[index].collidedLayers ||
				collisions[index].boundary)
			{
				if (collisions[index].collidedLayers & (1 << Engine::LAYER_ID_PLAYER))
					DamagePlayer();

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

	void UpdateAliens(Engine::ComponentManager* const manager)
	{
		_AlienManager *const data = reinterpret_cast<_AlienManager*>(manager->customData);

		const float delay = 0.1f + (manager->components.Size() / 50.f);

		if (Engine::Time() - data->lastMoved > delay)
		{
			data->lastMoved = Engine::Time();
			const uint16_t count = manager->components.Size();

			const float bombDropChance = 1.5f / static_cast<float>(count + 10);
			
			Engine::Layer *const layer = Engine::ResolveLayer(Engine::LAYER_ID_ALIEN);

			const float min_x = layer->boundingBox.center.x - layer->boundingBox.halfSize.x;
			const float max_x = layer->boundingBox.center.x + layer->boundingBox.halfSize.x;
			data->shouldChangeDirection =
				(data->moveDirection < 0 && min_x < 16.f) ||
				(data->moveDirection > 0 && max_x > Engine::g_context->config->screen_width - 16.f);

			if (data->shouldChangeDirection)
				data->moveDirection = -data->moveDirection;

			const float move_x = data->shouldChangeDirection ? 0.f : 32.f * data->moveDirection;
			const float move_y = data->shouldChangeDirection ? 32.f : 0.f;

			const uint16_t *const indexes = manager->components.Indexes();
			Alien *const aliens = manager->components.Data<Alien>();
			Engine::Transform *const transforms = Engine::ResolveModelTransformData(*layer);

			for (uint16_t i = 0; i < count; ++i)
			{
				const uint16_t index = indexes[i];
				const uint16_t modelIndex = aliens[index].model.index;

				transforms[modelIndex].position.x += move_x;
				transforms[modelIndex].position.y += move_y;
				
				if (Engine::Time() - aliens[index].lastDroped > 1.f &&
					Engine::Random() < bombDropChance)
				{
					aliens[index].lastDroped = Engine::Time();
					SpawnBomb(transforms[modelIndex].position.x, transforms[modelIndex].position.y);
				}
			}
		}
	}

	void SpawnAliens(Engine::ComponentManager* const manager)
	{
		if (manager->components.Size() > 0)
			return;

		_AlienManager *const data = reinterpret_cast<_AlienManager*>(manager->customData);

		Engine::Layer *const layer = Engine::ResolveLayer(Engine::LAYER_ID_ALIEN);
		Engine::Transform *const transforms = Engine::ResolveModelTransformData(*layer);
		Engine::Collider *const colliders = Engine::ResolveModelColliderData(*layer);

		data->moveDirection = 1;
		data->shouldChangeDirection = false;
		data->lastMoved = Engine::FrameData().time;

		for (int16_t i = 0; i < 11; i ++)
		{
			for (int16_t j = 0; j < 5; j++)
			{
				const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LAYER_ID_ALIEN);
				Engine::ResourceHandle sprite = reinterpret_cast<_AlienManager*>(manager->customData)->sprite[j < 2];
				const Engine::ComponentHandle model = Engine::CreateModel(entity, sprite);

				Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_ALIEN, model);
				
				Alien *const componentData = manager->components.Resolve<Alien>(component.index);
				componentData->lastDroped = 0.f;

				transforms[model.index].position = { i * 35.f, j * 35.f + 100.f };

				colliders[model.index].localBb.center = { 16.f, 16.f };
				colliders[model.index].localBb.halfSize = { 15.f, 11.f };
			}
		}
	}

	void ShutdownAlienManager(Engine::ComponentManager* const manager)
	{
		delete manager->customData;
	}
}

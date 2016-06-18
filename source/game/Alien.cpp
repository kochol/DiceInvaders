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
		int currentSprite;
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
		data->currentSprite = 0;
		data->moveDirection = 1;
		data->shouldChangeDirection = false;

		manager->customData = data;
	}

	void HandleAlienCollisions(Engine::ComponentManager* const manager)
	{
		uint16_t count = manager->components.Size();

		Engine::Layer &layer = Engine::ResolveLayer(Engine::LAYER_ID_ALIEN);
		const uint16_t *const indexes = Engine::ResolveModelIndexes(layer);
		Engine::BaseComponent *const model_components = Engine::ResolveModelComponentData(layer);
		Engine::Collision *const collisions = Engine::ResolveModelCollisionData(layer);;

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];

			if (collisions[index].collidedLayer != Engine::LAYER_ID_NONE ||
				collisions[index].out.any)
			{
				if (collisions[index].collidedLayer == Engine::LAYER_ID_PLAYER)
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
			
			data->currentSprite = 1 - data->currentSprite;

			const float move_x = data->shouldChangeDirection ? 0.f : 32.f * data->moveDirection;
			const float move_y = data->shouldChangeDirection ? 32.f : 0.f;

			const Engine::ResourceHandle newSprite = data->sprite[data->currentSprite];


			const uint16_t *const indexes = manager->components.Indexes();
			Alien *const aliens = manager->components.Data<Alien>();

			Engine::Layer &layer = Engine::ResolveLayer(Engine::LAYER_ID_ALIEN);
			Engine::Transform *const transforms = Engine::ResolveModelTransformData(layer);
			Engine::ResourceHandle *const sprites = Engine::ResolveModelSpriteData(layer);

			float min_x = static_cast<float>(Engine::GetConfig().screen_height);
			float max_x = 0.f;
			float min_y = static_cast<float>(Engine::GetConfig().screen_height);
			float max_y = 0.f;

			for (uint16_t i = 0; i < count; ++i)
			{
				const uint16_t index = indexes[i];
				const uint16_t modelIndex = aliens[index].model.index;

				transforms[modelIndex].position.x += move_x;
				transforms[modelIndex].position.y += move_y;
				sprites[modelIndex] = newSprite;

				min_x = min(min_x, transforms[modelIndex].position.x);
				max_x = max(max_x, transforms[modelIndex].position.x);
				min_y = min(min_y, transforms[modelIndex].position.y);
				max_y = max(max_y, transforms[modelIndex].position.y);

				const float rnd = Engine::Random();
				if (Engine::Time() - aliens[index].lastDroped > 1.f &&
					Engine::Random() < bombDropChance)
				{
					aliens[index].lastDroped = Engine::Time();
					SpawnBomb(transforms[modelIndex].position.x, transforms[modelIndex].position.y);
				}
			}

			data->shouldChangeDirection =
				(data->moveDirection < 0 && min_x < 0.5f) ||
				(data->moveDirection > 0 && max_x > Engine::g_context->config->screen_width - 64.f);

			if (data->shouldChangeDirection)
				data->moveDirection = -data->moveDirection;
		}
	}

	void SpawnAliens(Engine::ComponentManager* const manager)
	{
		if (manager->components.Size() > 0)
			return;

		_AlienManager *const data = reinterpret_cast<_AlienManager*>(manager->customData);

		Engine::Layer &layer = Engine::ResolveLayer(Engine::LAYER_ID_ALIEN);
		Engine::Transform *const transforms = Engine::ResolveModelTransformData(layer);

		data->moveDirection = 1;
		data->shouldChangeDirection = false;
		data->lastMoved = Engine::FrameData().time;

		for (int16_t i = 0; i < 11; i ++)
		{
			for (int16_t j = 0; j < 5; j++)
			{
				const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LAYER_ID_ALIEN);
				Engine::ResourceHandle sprite = reinterpret_cast<_AlienManager*>(manager->customData)->sprite[0];
				const Engine::ComponentHandle model = Engine::CreateModel(entity, sprite);

				Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_ALIEN, model);
				
				Alien *const componentData = manager->components.Resolve<Alien>(component.index);
				componentData->lastDroped = 0.f;

				transforms[model.index].position.x = static_cast<float>(i * 35);
				transforms[model.index].position.y = static_cast<float>(j * 35 + 100);
			}
		}
	}

	void ShutdownAlienManager(Engine::ComponentManager* const manager)
	{
		delete manager->customData;
	}
}

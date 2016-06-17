#include "Alien.h"
#include <algorithm>
#include <cassert>

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

		_AlienManager *const data = new _AlienManager;
		
		data->sprite[0] = Engine::LoadSprite("data/enemy1.bmp");
		data->sprite[1] = Engine::LoadSprite("data/enemy2.bmp");
		data->lastMoved = 0.f;
		data->currentSprite = 0;
		data->moveDirection = 1;
		data->shouldChangeDirection = false;

		manager->customData = data;
	}

	void UpdateAliens(Engine::ComponentManager* const manager)
	{
		_AlienManager *const data = reinterpret_cast<_AlienManager*>(manager->customData);
		const Engine::FrameData *const frame_data = Engine::g_context->frame_data;

		if (frame_data->time - data->lastMoved > 0.1f)
		{
			auto layerCollisions = frame_data->collisions.find(Engine::LayerId::ALIEN);

			if (layerCollisions != frame_data->collisions.end())
			{
				for (auto entity : layerCollisions->second)
				{
					Engine::ComponentHandle component = manager->componentMap[entity];

					Alien *const alien = manager->components.Resolve<Alien>(component.index);

					Engine::DestroyModel(alien->model);
					Engine::DestroyEntity(entity);
					manager->components.Free(component.index);
					manager->componentMap.erase(entity);
				}
			}

			const uint16_t count = manager->components.Size();

			data->lastMoved = frame_data->time;
			data->currentSprite = 1 - data->currentSprite;

			const uint16_t *const indexes = manager->components.Indexes();
			Alien *const aliens = manager->components.Data<Alien>();

			Engine::Layer *const layer = Engine::ResolveLayer(Engine::LayerId::ALIEN);
			Engine::Transform *const transforms = layer->models.Data<Engine::Transform>(0);
			Engine::ResourceHandle *const sprites = layer->models.Data<Engine::ResourceHandle>(1);

			const int16_t move_x = data->shouldChangeDirection ? 0 : 35 * data->moveDirection;
			const int16_t move_y = data->shouldChangeDirection ? 35 : 0;
			//const Engine::ResourceHandle newSprite = data->sprite[data->currentSprite];

			int min_x = Engine::g_context->config->screen_width;
			int max_x = 0;
			int min_y = Engine::g_context->config->screen_height;
			int max_y = 0;

			for (uint16_t i = 0; i < count; ++i)
			{
				const uint16_t index = indexes[i];
				const uint16_t modelIndex = aliens[index].model.index;

				transforms[modelIndex].x += move_x;
				transforms[modelIndex].y += move_y;
				//sprites[modelIndex] = newSprite;

				min_x = min(min_x, transforms[modelIndex].x);
				max_x = max(max_x, transforms[modelIndex].x);
				min_y = min(min_y, transforms[modelIndex].y);
				max_y = max(max_y, transforms[modelIndex].y);
			}

			data->shouldChangeDirection =
				(data->moveDirection < 0 && min_x < 35) ||
				(data->moveDirection > 0 && max_x > Engine::g_context->config->screen_width - 52.5f);

			if (data->shouldChangeDirection)
				data->moveDirection = -data->moveDirection;
		}
	}

	void SpawnAliens(Engine::ComponentManager* const manager)
	{
		if (manager->components.Size() > 0)
			return;

		for (int16_t i = 0; i < 10; i ++)
		{
			for (int16_t j = 0; j < 10; j++)
			{
				const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LayerId::ALIEN);
				Engine::ResourceHandle sprite = reinterpret_cast<_AlienManager*>(manager->customData)->sprite[0];
				const Engine::ComponentHandle model = Engine::CreateModel(entity, sprite);

				Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::ComponentType::ALIEN);
				
				Alien *const componentData = manager->components.Resolve<Alien>(component.index);
				componentData->model = model;
				componentData->lastDroped = 0.f;

				Engine::Transform *const transform = Engine::ResolveTransform(model);

				transform->x = i * 35;
				transform->y = j * 35;
			}
		}
	}

	void ShutdownAliens(Engine::ComponentManager* const manager)
	{
		delete manager->customData;
	}
}

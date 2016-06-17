#include "Bomb.h"
#include "Game.h"

namespace Game
{
	struct _BombManager
	{
		Engine::ResourceHandle sprite;
	};

	void InitBombManager(Engine::ComponentManager *const manager)
	{
		manager->components.Init(100, { sizeof(Bomb) });

		_BombManager *const data = new _BombManager;

		data->sprite = Engine::LoadSprite("data/bomb.bmp");

		manager->customData = data;
	}

	void HandleBombCollisions(Engine::ComponentManager* const manager)
	{
		uint16_t count = manager->components.Size();

		Engine::Layer *const layer = Engine::ResolveLayer(Engine::LayerId::BOMB);
		const uint16_t *const indexes = layer->models.Indexes();
		Engine::Transform *const transforms = layer->models.Data<Engine::Transform>(0);
		Engine::Collider *const collisions = layer->models.Data<Engine::Collider>(2);


		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];

			if (collisions[index].collidedLayer != Engine::LayerId::NONE ||
				collisions[index].out.top)
			{

				Engine::ComponentHandle component = manager->componentMap[transforms[index].entity];

				Engine::ComponentHandle model = Engine::GetComponentHandle(
					index,
					Engine::ComponentType::MODEL,
					component.header.layer);

				Engine::DestroyModel(model);
				Engine::DestroyEntity(transforms[index].entity);
				manager->components.Free(component.index);
				manager->componentMap.erase(transforms[index].entity);
				--i;
				--count;
			}
		}
	}

	void UpdateBombs(Engine::ComponentManager* const manager)
	{
		const Engine::FrameData *const frame_data = Engine::g_context->frame_data;

		const uint16_t count = manager->components.Size();

		const uint16_t *const indexes = manager->components.Indexes();
		Bomb *const rockets = manager->components.Data<Bomb>();

		Engine::Layer *const layer = Engine::ResolveLayer(Engine::LayerId::BOMB);
		Engine::Transform *const transforms = layer->models.Data<Engine::Transform>(0);

		const float move = frame_data->dt * 200.f;

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const uint16_t modelIndex = rockets[index].model.index;

			transforms[modelIndex].y += move;
		}
	}

	void ShutdownBombManager(Engine::ComponentManager* const manager)
	{
		delete manager->customData;
	}

	Engine::EntityHandle SpawnBomb(float x, float y)
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::ComponentType::BOMB);
		const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LayerId::BOMB);

		Engine::ResourceHandle sprite = reinterpret_cast<_BombManager*>(manager->customData)->sprite;
		const Engine::ComponentHandle model = Engine::CreateModel(entity, sprite);

		Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::ComponentType::BOMB);

		Engine::BaseComponent *const componentData = manager->components.Resolve<Engine::BaseComponent>(component.index);
		componentData->model = model;

		Engine::Transform *const transform = Engine::ResolveTransform(model);
		transform->x = x;
		transform->y = y;

		return entity;
	}
}

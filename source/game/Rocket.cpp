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
		data->sprite = Engine::LoadSprite("data/rocket.bmp");

		manager->customData = data;
	}

	void HandleRocketCollisions(Engine::ComponentManager* const manager)
	{
		uint16_t count = manager->components.Size();

		Engine::Layer *const layer = Engine::ResolveLayer(Engine::LayerId::ROCKET);
		const uint16_t *const indexes = layer->models.Indexes();
		Engine::Transform *const transforms = layer->models.Data<Engine::Transform>(0);
		Engine::CollisionInfo *const collisions = layer->models.Data<Engine::CollisionInfo>(2);


		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];

			if (collisions[index].collidedLayer != Engine::LayerId::NONE ||
				collisions[index].out.top)
			{
				if(collisions[index].collidedLayer == Engine::LayerId::ALIEN)
					ScorePlayer();

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

	void UpdateRockets(Engine::ComponentManager* const manager)
	{
		char debug_text[100];
		sprintf(debug_text, "rockets: %d", manager->components.Size());
		Engine::g_context->system->drawText(0, 0, debug_text);

		const Engine::FrameData *const frame_data = Engine::g_context->frame_data;

		const uint16_t count = manager->components.Size();

		const uint16_t *const indexes = manager->components.Indexes();
		Rocket *const rockets = manager->components.Data<Rocket>();

		Engine::Layer *const layer = Engine::ResolveLayer(Engine::LayerId::ROCKET);
		Engine::Transform *const transforms = layer->models.Data<Engine::Transform>(0);

		const float move = frame_data->dt * 1500.f;

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const uint16_t modelIndex = rockets[index].model.index;

			transforms[modelIndex].y -= move;
		}
	}

	void ShutdownRocketManager(Engine::ComponentManager* const manager)
	{
		delete manager->customData;
	}

	Engine::EntityHandle SpawnRocket(float x, float y)
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::ComponentType::ROCKET);
		const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LayerId::ROCKET);

		Engine::ResourceHandle sprite = reinterpret_cast<_RocketManager*>(manager->customData)->sprite;
		const Engine::ComponentHandle model = Engine::CreateModel(entity, sprite);

		Engine::ComponentHandle component = Engine::CreateComponent(entity, Engine::ComponentType::ROCKET);

		Rocket *const componentData = manager->components.Resolve<Rocket>(component.index);
		componentData->model = model;

		Engine::Transform *const transform = Engine::ResolveTransform(model);

		transform->x = x;
		transform->y = y;

		return entity;
	}
}
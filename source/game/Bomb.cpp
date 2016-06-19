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

		data->sprite = Engine::LoadSprite("bomb.bmp");

		manager->customData = data;
	}

	void HandleBombCollisions(Engine::ComponentManager* const manager)
	{
		uint16_t count = manager->components.Size();

		const Engine::LayerId layer = Engine::LAYER_ID_BOMB;
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

	void UpdateBombs(Engine::ComponentManager* const manager)
	{
		const uint16_t count = manager->components.Size();

		const uint16_t *const indexes = manager->components.Indexes();
		Bomb *const bombs = manager->components.Data<Bomb>();

		Engine::Transform *const transforms = Engine::ResolveModelTransformData(Engine::LAYER_ID_BOMB);

		const float move = Engine::DeltaTime() * 200.f;

		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const uint16_t modelIndex = bombs[index].model.index;

			transforms[modelIndex].position.y += move;
		}
	}

	void ShutdownBombManager(Engine::ComponentManager* const manager)
	{
		delete manager->customData;
	}

	Engine::EntityHandle SpawnBomb(float x, float y)
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(Engine::COMPONENT_TYPE_BOMB);
		const Engine::EntityHandle entity = Engine::CreateEntity(Engine::LAYER_ID_BOMB);

		Engine::ResourceHandle sprite = reinterpret_cast<_BombManager*>(manager->customData)->sprite;
		const Engine::ComponentHandle model = Engine::CreateModel(entity, sprite);

		Engine::CreateComponent(entity, Engine::COMPONENT_TYPE_BOMB, model);

		Engine::ResolveModelTransform(model)->position = { x, y };

		Engine::Collider *const collider = Engine::ResolveModelCollider(model);
		collider->localBb.center = { 16.f, 16.f };
		collider->localBb.halfSize = { 3.f, 8.f };

		return entity;
	}
}

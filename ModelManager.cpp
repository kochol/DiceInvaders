#include "Engine.h"
#include <cassert>

#include "DiceInvaders.h"

namespace Engine
{
	ComponentHandle CreateModel(World* const world, const EntityHandle handle, const Sprite sprite)
	{
		const ComponentHandle compHandle = { world->transforms.size(), 0 };

		world->transforms.push_back({ handle, 0.f, 0.f });
		world->sprites.push_back(sprite);
		world->layers.push_back(0);

		return compHandle;
	}

	ComponentHandle LookupModel(const World &world, const EntityHandle handle)
	{
		const auto compHandle = world.modelMap.find(handle);
		assert(compHandle != world.modelMap.end());
		return compHandle->second;
	}

	void DestroyModel(World *const world, const ComponentHandle handle)
	{
		// TODO: make it deferred
		assert(handle.index < world->modelMap.size());

		const EntityHandle oldEntityHandle = world->transforms[handle.index].handle;
		world->modelMap.erase(oldEntityHandle);

		const uint32_t lastIndex = world->transforms.size() - 1;
		world->transforms[handle.index] = world->transforms[lastIndex];
		world->sprites[handle.index] = world->sprites[lastIndex];
		world->layers[handle.index] = world->layers[lastIndex];

		const EntityHandle newEntityHandle = world->transforms[handle.index].handle;
		world->modelMap[newEntityHandle] = handle;

		// TODO: update other comp model handles
	}

	void DrawModels(const World& world)
	{
		assert(world.transforms.size() == world.sprites.size());

		const uint16_t count = world.transforms.size();
		for (uint16_t i = 0; i < count; ++i)
		{
			world.sprites[i].sprite->draw(world.transforms[i].x, world.transforms[i].y);
		}
	}

	void DetectCollisions(const World& world, std::vector<CollisionInfo>* collisions)
	{
		assert(collisions != nullptr && collisions->empty());

		const int size = 32;
		const uint16_t count = world.transforms.size();

		for (uint16_t i = 0; i < count; ++i)
		{
			for (uint16_t j = 0; j < i; ++j)
			{
				if (world.transforms[i].x < world.transforms[j].x + size &&
					world.transforms[i].x + size > world.transforms[j].x &&
					world.transforms[i].y < world.transforms[j].y + size &&
					world.transforms[i].y + size > world.transforms[j].y)
				{
					collisions->push_back(CollisionInfo{ i, j });
				}

			}
		}
	}
}

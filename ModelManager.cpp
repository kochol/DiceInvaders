#include "ModelManager.h"
#include <cassert>

#include "DiceInvaders.h"

namespace DiceInvaders
{
	ComponentHandle CreateModel(World* const world, const EntityHandle handle, const Sprite sprite)
	{
		const ComponentHandle compHandle = { world->transforms.size(), 0 };

		world->transforms.push_back({handle, 0.f, 0.f});
		world->sprites.push_back(sprite);
		world->layers.push_back(0);

		return handle;
	}

	void DrawModels(const std::vector<Transform>& transforms, std::vector<Sprite>& sprites)
	{
		assert(transforms.size() == sprites.size());

		const uint16_t count = transforms.size();
		for (uint16_t i = 0; i < count; ++i)
		{
			sprites[i].sprite->draw(transforms[i].x, transforms[i].y);
		}
	}

	void DetectCollisions(const std::vector<Transform>& transforms, std::vector<CollisionInfo>* collisions)
	{
		assert(collisions != nullptr && collisions->empty());

		const int size = 32;
		const uint16_t count = transforms.size();

		for (uint16_t i = 0; i < count; ++i)
		{
			for (uint16_t j = 0; j < i; ++j)
			{
				if (transforms[i].x < transforms[j].x + size &&
					transforms[i].x + size > transforms[j].x &&
					transforms[i].y < transforms[j].y + size &&
					transforms[i].y + size > transforms[j].y)
				{
					collisions->push_back(CollisionInfo{ i, j });
				}

			}
		}
	}

	ResourceHandle LoadSprite(IDiceInvaders *const system, Resources *const resources, const std::string& name)
	{
		ISprite *const sprite = system->createSprite(name.c_str());
		assert(sprite != nullptr);

		const ResourceHandle handle = resources->handleManager.Create();
		resources->spriteMap[handle] = { sprite };

		return handle;
	}

	Sprite LookupSprite(const Resources& resources, const ResourceHandle handle)
	{
		const auto sprite = resources.spriteMap.find(handle);
		assert(sprite != resources.spriteMap.end());

		return sprite->second;
	}

	void DestroySprite(const IDiceInvaders *const system, Resources *const resources, ResourceHandle handle)
	{
		const auto sprite = resources->spriteMap.find(handle);
		assert(sprite != resources->spriteMap.end());

		sprite->second.sprite->destroy();
		resources->spriteMap.erase(handle);
		resources->handleManager.Destroy(handle);
	}
}

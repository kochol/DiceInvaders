#include "Engine.h"
#include <cassert>

namespace Engine
{
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

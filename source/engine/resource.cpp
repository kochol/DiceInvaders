/* ---------------------------------------------------------------------------
**
** resource.cpp
** Asset management stuff, implemented as a part of "Model" component
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#include "engine.h"

#include <cassert>

namespace Engine
{
	ResourceHandle LoadSprite(const std::string& name)
	{
		ILibrary *const system = g_context->system;
		Resources *const resources = g_context->resources;

		const std::string fullName = "../data/" + name;
		ISprite *const sprite = system->createSprite(fullName.c_str());
		assert(sprite != nullptr);

		ResourceHandle handle;
		handle.index = resources->caches[RESOURCE_TYPE_SPRITE].Alloc();
		handle.header.type = RESOURCE_TYPE_SPRITE;
		handle.header.padding = 0;

		*ResolveSprite(handle) = sprite;

		return handle;
	}

	void CleanupResources()
	{
		Resources *const resources = g_context->resources;

		for (const auto resource : resources->to_be_freed)
		{
			ISprite *const sprite = *ResolveSprite(resource);
			sprite->destroy();

			resources->caches[RESOURCE_TYPE_SPRITE].Free(resource.index);
		}
		resources->to_be_freed.clear();
	}

	void UnloadResources()
	{
		for (auto &pool : g_context->resources->caches)
		{
			const uint16_t count = pool.Size();
			if (count == 0)
				continue;

			const uint16_t *const indexes = pool.Indexes();
			ISprite **const i_sprites = pool.Data<ISprite*>();

			for (unsigned i = 0; i < count; i++)
			{
				uint16_t index = indexes[i];
				i_sprites[index]->destroy();
			}
		}
	}
}

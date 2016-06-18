#include "Engine.h"
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

		ResolveSprite(handle) = sprite;

		return handle;
	}

	void DestroySprite(ResourceHandle handle)
	{
		Resources *const resources = g_context->resources;

		ISprite *const sprite = ResolveSprite(handle);
		sprite->destroy();

		resources->caches[RESOURCE_TYPE_SPRITE].Free(handle.index);
	}
}

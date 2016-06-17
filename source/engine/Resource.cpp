#include "Engine.h"
#include <cassert>

namespace Engine
{
	ResourceHandle LoadSprite(const std::string& name)
	{
		ILibrary *const system = g_context->system;
		Resources *const resources = g_context->resources;

		ISprite *const sprite = system->createSprite(name.c_str());
		assert(sprite != nullptr);

		ResourceHandle handle;
		handle.index = resources->handleManager.Alloc();
		handle.header.type = ResourceType::SPRITE;
		handle.header.padding = 0;
		resources->spriteMap[handle] = sprite;

		return handle;
	}

	void DestroySprite(ResourceHandle handle)
	{
		Resources *const resources = g_context->resources;

		resources->spriteMap[handle]->destroy();
		resources->spriteMap.erase(handle);
		resources->handleManager.Free(handle.index);
	}
}

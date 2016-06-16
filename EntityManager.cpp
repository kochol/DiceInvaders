#include "Engine.h"

namespace Engine
{
	EntityHandle CreateEntity(const LayerId layer)
	{
		return g_context->world->handleManager.Create(layer);
	}

	void DestroyEntity(const EntityHandle handle)
	{
		g_context->world->handleManager.Destroy(handle);
	}

	bool ValidEntity(const EntityHandle handle)
	{
		return g_context->world->handleManager.Valid(handle);
	}
}
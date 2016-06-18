#include "Engine.h"
#include <cassert>

namespace Engine
{
	ComponentHandle CreateModel(const EntityHandle& handle, const ResourceHandle& sprite)
	{
		World *const world = g_context->world;

		assert(world->layers[handle.header.layer] != nullptr);

		Layer *const layer = ResolveLayer(handle);

		ComponentHandle compHandle;
		compHandle.index = layer->models.Alloc();
		compHandle.header.type = COMPONENT_TYPE_MODEL;
		compHandle.header.layer = handle.header.layer;

		*ResolveModelComponent(compHandle) = { handle, compHandle };
		ResolveModelTransform(compHandle)->position = { 0.f, 0.f };
		*ResolveModelSprite(compHandle) = sprite;
		Collider *collider = ResolveModelCollider(compHandle);
		collider->localBb.center = { 16.f, 16.f };
		collider->localBb.halfSize = { 16.f, 16.f };
		*ResolveModelCollision(compHandle) = { LAYER_ID_NONE };
		
		world->modelMap[handle] = compHandle;

		return compHandle;
	}

	void DestroyModel(const ComponentHandle& handle)
	{
		assert(handle.header.type == COMPONENT_TYPE_MODEL);

		World *const world = g_context->world;

		// TODO: make it deferred

		const BaseComponent *const component = ResolveModelComponent(handle);
		world->modelMap.erase(component->entity);

		Layer *const layer = ResolveLayer(handle);

		layer->models.Free(handle.index);
	}

	void Render()
	{
		const World *const world = g_context->world;

		ISprite **const iSprites = ResolveSpriteData();
		const uint16_t *const iSpritesIndexes = ResolveSpriteIndexes();

		for (const auto layer : world->layers)
		{
			if (layer == nullptr)
				continue;

			const uint16_t count = ResolveModelCount(*layer);

			const uint16_t *const indexes = ResolveModelIndexes(*layer);
			const Transform *const transforms = ResolveModelTransformData(*layer);
			const ResourceHandle *const sprites = ResolveModelSpriteData(*layer);

			for (uint16_t i = 0; i < count; ++i)
			{
				const uint16_t index = indexes[i];
				const uint16_t iSpriteIndex = iSpritesIndexes[sprites[index].index];
				ISprite *const sprite = iSprites[iSpriteIndex];
				sprite->draw(
					static_cast<int>(transforms[index].position.x),
					static_cast<int>(transforms[index].position.y));
			}
		}
	}
}

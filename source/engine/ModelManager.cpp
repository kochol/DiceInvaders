#include "Engine.h"
#include <cassert>

namespace Engine
{
	ComponentHandle CreateModel(const EntityHandle handle, const ResourceHandle sprite)
	{
		World *const world = g_context->world;

		assert(world->layers.find(handle.header.layer) != world->layers.end());

		Layer *const layer = ResolveLayer(handle);

		ComponentHandle compHandle;
		compHandle.index = layer->models.Alloc();
		compHandle.header.type = ComponentType::MODEL;
		compHandle.header.layer = handle.header.layer;

		*layer->models.Resolve<Transform>(compHandle.index, 0) = { handle, 0, 0 };
		*layer->models.Resolve<ResourceHandle>(compHandle.index, 1) = sprite;

		Collider *const collision_info = layer->models.Resolve<Collider>(compHandle.index, 2);
		collision_info->collidedLayer = LayerId::NONE;
		collision_info->out.any = 0;
		
		world->modelMap[handle] = compHandle;

		return compHandle;
	}

	void DestroyModel(const ComponentHandle handle)
	{
		assert(handle.header.type == ComponentType::MODEL);

		World *const world = g_context->world;

		// TODO: make it deferred

		const Transform *const transform = ResolveTransform(handle);
		world->modelMap.erase(transform->entity);

		Layer *const layer = ResolveLayer(handle);

		layer->models.Free(handle.index);
	}

	void Render()
	{
		const World *const world = g_context->world;

		ISprite **const iSprites = g_context->resources->spriteCache.Data<ISprite*>();
		const uint16_t *const iSpritesIndexes = g_context->resources->spriteCache.Indexes();

		for (const auto layerPair : world->layers)
		{
			const Layer *const layer = layerPair.second;

			const uint16_t count = layer->models.Size();

			const uint16_t* indexes = layer->models.Indexes();
			const Transform *const transforms = layer->models.Data<Transform>(0);
			const ResourceHandle *const sprites = layer->models.Data<ResourceHandle>(1);

			for (uint16_t i = 0; i < count; ++i)
			{
				const uint16_t index = indexes[i];
				const uint16_t iSpriteIndex = iSpritesIndexes[sprites[index].index];
				ISprite *const sprite = iSprites[iSpriteIndex];
				sprite->draw(static_cast<int>(transforms[index].x), static_cast<int>(transforms[index].y));
			}
		}
	}

	void DetectCollisions()
	{
		World *const world = g_context->world;

		const int size = 32;

		for (const auto collisionMask : world->collisionMasks)
		{
			Layer *const first_layer = world->layers[collisionMask.first];
			Layer *const second_layer = world->layers[collisionMask.second];

			const uint16_t first_count = first_layer->models.Size();
			const uint16_t second_count = second_layer->models.Size();

			const uint16_t *const first_indexes = first_layer->models.Indexes();
			const uint16_t *const second_indexes = second_layer->models.Indexes();

			Transform *const first_transforms = first_layer->models.Data<Transform>(0);
			Transform *const second_transforms = second_layer->models.Data<Transform>(0);

			Collider *const first_collisions = first_layer->models.Data<Collider>(2);
			Collider *const second_collisions = second_layer->models.Data<Collider>(2);

			const int screen_width = g_context->config->screen_width;
			const int screen_height = g_context->config->screen_height;
			
			for (uint16_t i = 0; i < first_count; ++i)
			{
				const uint16_t first_index = first_indexes[i];
				const Transform *const first_transform = first_transforms + first_index;

				first_collisions[first_index].out.left = first_transform->x < 0;
				first_collisions[first_index].out.right = first_transform->x > screen_width;
				first_collisions[first_index].out.top = first_transform->y < 0;
				first_collisions[first_index].out.down = first_transform->y > screen_height;

				for (uint16_t j = 0; j < second_count; ++j)
				{
					const uint16_t second_index = second_indexes[j];
					const Transform *const second_transform = second_transforms + second_index;

					second_collisions[second_index].out.left = second_transform->x < 0;
					second_collisions[second_index].out.right = second_transform->x > screen_width;
					second_collisions[second_index].out.top = second_transform->y < 0;
					second_collisions[second_index].out.down = second_transform->y > screen_height;

					const bool collided =
						(first_transform->x < second_transform->x + size &&
							first_transform->x + size > second_transform->x &&
							first_transform->y < second_transform->y + size &&
							first_transform->y + size > second_transform->y);

					if (collided)
					{
						first_collisions[first_index].collidedLayer = collisionMask.second;
						second_collisions[second_index].collidedLayer = collisionMask.first;
					}
				}
			}
		}
	}
}

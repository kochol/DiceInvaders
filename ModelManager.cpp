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

		*layer->models.Resolve<Transform>(compHandle.index, 0) = { handle, 0.f, 0.f };
		*layer->models.Resolve<ResourceHandle>(compHandle.index, 1) = sprite;
		
		world->modelMap[handle] = compHandle;

		return compHandle;
	}

	void DestroyModel(const ComponentHandle handle)
	{
		World *const world = g_context->world;

		// TODO: make it deferred
		assert(handle.index < world->modelMap.size());

		const Transform *const transform = ResolveTransform(handle);
		world->modelMap.erase(transform->handle);

		Layer *const layer = ResolveLayer(handle);

		layer->models.Free(handle.index);
	}

	void Render()
	{
		const World *const world = g_context->world;

		for (const auto layerPair : world->layers)
		{
			const Layer *const layer = layerPair.second;

			const uint16_t* indexes = layer->models.Indexes();
			const Transform *const transforms = layer->models.Data<Transform>(0);
			const ResourceHandle *const sprites = layer->models.Data<ResourceHandle>(1);
			const uint16_t count = layer->models.Size();

			for (uint16_t i = 0; i < count; ++i)
			{
				const uint16_t index = indexes[i];
				ISprite *const sprite = LookupSprite(sprites[index]);
				sprite->draw(transforms[index].x, transforms[index].y);
			}
		}
	}

	void DetectCollisions()
	{
		World *const world = g_context->world;

		std::vector<CollisionInfo> *const collisions = &g_context->frame_data->collisions;

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
			
			for (uint16_t i = 0; i < first_count; ++i)
			{
				const uint16_t first_index = first_indexes[i];
				const Transform *const first_transform = first_transforms + first_index;

				for (uint16_t j = 0; j < second_count; ++j)
				{
					const uint16_t second_index = second_indexes[j];
					const Transform *const second_transform = second_transforms + second_index;

					if (first_transform->x < second_transform->x + size &&
						first_transform->x + size > second_transform->x &&
						first_transform->y < second_transform->y + size &&
						first_transform->y + size > second_transform->y)
					{
						const CollisionInfo collision_info =
						{
							first_transform->handle,
							second_transform->handle
						};

						collisions->push_back(collision_info);
					}
				}
			}
		}
	}
}

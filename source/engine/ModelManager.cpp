#include "Engine.h"
#include <cassert>

namespace Engine
{
	ComponentHandle CreateModel(const EntityHandle handle, const ResourceHandle sprite)
	{
		World *const world = g_context->world;

		assert(world->layers[handle.header.layer] != nullptr);

		Layer& layer = ResolveLayer(handle);

		ComponentHandle compHandle;
		compHandle.index = layer.models.Alloc();
		compHandle.header.type = COMPONENT_TYPE_MODEL;
		compHandle.header.layer = handle.header.layer;

		ResolveModelComponent(compHandle) = { handle, compHandle };
		ResolveModelTransform(compHandle) = { { 0.f, 0.f } };
		ResolveModelSprite(compHandle) = sprite;
		ResolveModelCollider(compHandle) = { { 0.f, 0.f, 32.f, 32.f } };
		ResolveModelCollision(compHandle) = { LAYER_ID_NONE };
		
		world->modelMap[handle] = compHandle;

		return compHandle;
	}

	void DestroyModel(const ComponentHandle handle)
	{
		assert(handle.header.type == COMPONENT_TYPE_MODEL);

		World *const world = g_context->world;

		// TODO: make it deferred

		const BaseComponent const component = ResolveModelComponent(handle);
		world->modelMap.erase(component.entity);

		Layer &layer = ResolveLayer(handle);

		layer.models.Free(handle.index);
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

	void DetectCollisions()
	{
		World *const world = g_context->world;

		const int screen_width = g_context->config->screen_width;
		const int screen_height = g_context->config->screen_height;

		for (const auto collisionMask : world->collisionMasks)
		{
			Layer &first_layer = ResolveLayer(collisionMask.first);
			Layer &second_layer = ResolveLayer(collisionMask.second);

			const uint16_t first_count = ResolveModelCount(first_layer);
			const uint16_t second_count = ResolveModelCount(second_layer);

			const uint16_t *const first_indexes = ResolveModelIndexes(first_layer);
			const uint16_t *const second_indexes = ResolveModelIndexes(second_layer);

			Transform *const first_transforms = ResolveModelTransformData(first_layer);
			Transform *const second_transforms = ResolveModelTransformData(second_layer);

			Collider *const first_colliders = ResolveModelColliderData(first_layer);
			Collider *const second_colliders = ResolveModelColliderData(second_layer);

			Collision *const first_collisions = ResolveModelCollisionData(first_layer);
			Collision *const second_collisions = ResolveModelCollisionData(second_layer);

			for (uint16_t i = 0; i < first_count; ++i)
			{
				const uint16_t first_index = first_indexes[i];
				const Transform *const first_transform = first_transforms + first_index;
				const Collider *const first_collider = first_colliders + first_index;
				Collision *const first_collision = first_collisions + first_index;

				first_collision->out.left = first_transform->position.x + first_collider->localBb.left < 0;
				first_collision->out.right = first_transform->position.x + first_collider->localBb.right > screen_width;
				first_collision->out.top = first_transform->position.y + first_collider->localBb.down < 0;
				first_collision->out.down = first_transform->position.y + first_collider->localBb.top > screen_height;

				for (uint16_t j = 0; j < second_count; ++j)
				{
					const uint16_t second_index = second_indexes[j];
					const Transform *const second_transform = second_transforms + second_index;
					const Collider *const second_collider = second_colliders + second_index;
					Collision *const second_collision = second_collisions + second_index;

					second_collision->out.left = second_transform->position.x + second_collider->localBb.left < 0;
					second_collision->out.right = second_transform->position.x + second_collider->localBb.right > screen_width;
					second_collision->out.top = second_transform->position.y + second_collider->localBb.down < 0;
					second_collision->out.down = second_transform->position.y + second_collider->localBb.top > screen_height;

					const bool collided =
						first_transform->position.x + first_collider->localBb.left < second_transform->position.x + second_collider->localBb.right &&
						first_transform->position.x + first_collider->localBb.right > second_transform->position.x + second_collider->localBb.left &&
						first_transform->position.y + first_collider->localBb.down < second_transform->position.y + second_collider->localBb.top &&
						first_transform->position.y + first_collider->localBb.top > second_transform->position.y + second_collider->localBb.down;

					if (collided)
					{
						first_collision->collidedLayer = collisionMask.second;
						second_collision->collidedLayer = collisionMask.first;
					}
				}
			}
		}
	}
}

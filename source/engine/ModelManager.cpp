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
		*ResolveModelTransform(compHandle) = { { 0.f, 0.f } };
		*ResolveModelSprite(compHandle) = sprite;
		*ResolveModelCollider(compHandle) = { { 16.f, 32.f, 16.f, 32.f } };
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


	inline bool CheckAabbCollision(
		const float x1, const float y1, const float w1, const float h1,
		const float x2, const float y2, const float w2, const float h2)
	{
		return
			abs(x1 - x2) * 2.f < w1 + w2 &&
			abs(y1 - y2) * 2.f < h1 + h2;
	}

	void UpdateBoundingBoxes()
	{
		World *const world = g_context->world;

		const float screen_width = ScreenWidth();
		const float screen_height = ScreenHeight();

		const float screen_center_x = screen_width / 2;
		const float screen_center_y = screen_height / 2;

		for (auto& layer : world->layers)
		{
			float min_x = screen_width;
			float max_x = 0;
			float min_y = screen_height;
			float max_y = 0;

			const uint16_t count = ResolveModelCount(*layer);

			const uint16_t *const indexes = ResolveModelIndexes(*layer);
			Transform *const transforms = ResolveModelTransformData(*layer);
			Collider *const colliders = ResolveModelColliderData(*layer);
			Collision *const collisions = ResolveModelCollisionData(*layer);

			for (uint16_t i = 0; i < count; ++i)
			{
				const uint16_t index = indexes[i];
				const Transform *const transform = transforms + index;
				const Collider *const collider = colliders + index;
				Collision *const collision = collisions + index;

				const float local_x = transform->position.x + collider->localBb.x;
				const float local_y = transform->position.y + collider->localBb.y;
				const float w = collider->localBb.w;
				const float h = collider->localBb.h;

				collision->boundary = !CheckAabbCollision(
					local_x, local_y, w, w,
					screen_center_x, screen_center_y, screen_width, screen_height);
			}

			layer->boundingBox.x = (min_x + max_x) / 2.f;
			layer->boundingBox.y = (min_y + max_y) / 2.f;
			layer->boundingBox.w = max_x - min_x;
			layer->boundingBox.h = max_y - min_y;
		}
	}

	void DetectCollisions()
	{
		World *const world = g_context->world;

		const float screen_width = ScreenWidth();
		const float screen_height = ScreenHeight();

		const float screen_center_x = screen_width / 2;
		const float screen_center_y = screen_height / 2;

		for (const auto collisionMask : world->collisionMasks)
		{
			Layer *const first_layer = ResolveLayer(collisionMask.first);
			Layer *const second_layer = ResolveLayer(collisionMask.second);

			const uint16_t first_count = ResolveModelCount(*first_layer);
			const uint16_t second_count = ResolveModelCount(*second_layer);

			const uint16_t *const first_indexes = ResolveModelIndexes(*first_layer);
			const uint16_t *const second_indexes = ResolveModelIndexes(*second_layer);

			Transform *const first_transforms = ResolveModelTransformData(*first_layer);
			Transform *const second_transforms = ResolveModelTransformData(*second_layer);

			Collider *const first_colliders = ResolveModelColliderData(*first_layer);
			Collider *const second_colliders = ResolveModelColliderData(*second_layer);

			Collision *const first_collisions = ResolveModelCollisionData(*first_layer);
			Collision *const second_collisions = ResolveModelCollisionData(*second_layer);

			for (uint16_t i = 0; i < first_count; ++i)
			{
				const uint16_t first_index = first_indexes[i];
				const Transform *const first_transform = first_transforms + first_index;
				const Collider *const first_collider = first_colliders + first_index;
				Collision *const first_collision = first_collisions + first_index;

				const float first_local_x = first_transform->position.x + first_collider->localBb.x;
				const float first_local_y = first_transform->position.y + first_collider->localBb.y;
				const float first_w = first_collider->localBb.w;
				const float first_h = first_collider->localBb.h;

				first_collision->boundary = !CheckAabbCollision(
					first_local_x, first_local_y, first_w, first_h,
					screen_center_x, screen_center_y, screen_width, screen_height);

				for (uint16_t j = 0; j < second_count; ++j)
				{
					const uint16_t second_index = second_indexes[j];
					const Transform *const second_transform = second_transforms + second_index;
					const Collider *const second_collider = second_colliders + second_index;
					Collision *const second_collision = second_collisions + second_index;

					const float second_local_x = second_transform->position.x + second_collider->localBb.x;
					const float second_local_y = second_transform->position.y + second_collider->localBb.y;
					const float second_w = second_collider->localBb.w;
					const float second_h = second_collider->localBb.h;

					second_collision->boundary = !CheckAabbCollision(
						second_local_x, second_local_y, second_w, second_h,
						screen_center_x, screen_center_y, screen_width, screen_height);

					const bool collided = CheckAabbCollision(
						first_local_x, first_local_y, first_w, first_h,
						second_local_x, second_local_y, second_w, second_h);

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

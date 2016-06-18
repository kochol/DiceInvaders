#include "Engine.h"

namespace Engine
{
	// Check wheter b is inside a
	inline bool CheckAabbInside(const BoundingBox& b1, const BoundingBox& b2);
	inline bool CheckAabbCollision(const Vector2& p1, const Vector2& p2, const BoundingBox& b1, const BoundingBox& b2);
	inline bool CheckAabbCollision(const BoundingBox& b1, const BoundingBox& b2);
	inline bool CheckAabbCollision(
		const float x1, const float y1, const float w1, const float h1,
		const float x2, const float y2, const float w2, const float h2);

	void UpdateLayerBoundingBoxes()
	{
		World *const world = g_context->world;

		const float screen_width = ScreenWidth();
		const float screen_height = ScreenHeight();

		const float size = 32.f;

		for (auto layer : world->layers)
		{
			if (layer == nullptr)
				continue;

			const uint16_t count = ResolveModelCount(*layer);

			if (count == 0)
			{
				layer->boundingBox.center = { 0.f, 0.f };
				layer->boundingBox.halfSize = { 0.f, 0.f };

				continue;
			}

			float min_x = screen_width;
			float max_x = 0;
			float min_y = screen_height;
			float max_y = 0;

			const uint16_t *const indexes = ResolveModelIndexes(*layer);
			Transform *const transforms = ResolveModelTransformData(*layer);

			for (uint16_t i = 0; i < count; ++i)
			{
				const uint16_t index = indexes[i];
				const Transform *const transform = transforms + index;

				const float x = transform->position.x;
				const float y = transform->position.y;

				min_x = min(min_x, x);
				max_x = max(max_x, x);
				min_y = min(min_y, y);
				max_y = max(max_y, y);

			}

			max_x += size;
			max_y += size;

			layer->boundingBox.center.x = (min_x + max_x) / 2.f;
			layer->boundingBox.center.y = (min_y + max_y) / 2.f;
			layer->boundingBox.halfSize.x = (max_x - min_x) / 2.f;
			layer->boundingBox.halfSize.y = (max_y - min_y) / 2.f;
		}
	}

	void DetectBoundaryCollisions()
	{
		World *const world = g_context->world;

		const float h_screen_width = ScreenWidth() / 2.f;
		const float h_screen_height = ScreenHeight() / 2.f;

		BoundingBox screenAabb;
		screenAabb.center = { h_screen_width, h_screen_height };
		screenAabb.halfSize = { h_screen_width, h_screen_height };

		for (auto layer : world->layers)
		{
			if (layer == nullptr)
				continue;

			bool broad_inside = CheckAabbInside(
				screenAabb,
				layer->boundingBox);
			if (broad_inside == true)
				continue;

			const uint16_t count = ResolveModelCount(*layer);

			const uint16_t *const indexes = ResolveModelIndexes(*layer);
			const Transform *const transforms = ResolveModelTransformData(*layer);
			Collision *const collisions = ResolveModelCollisionData(*layer);

			for (uint16_t i = 0; i < count; ++i)
			{
				const uint16_t index = indexes[i];
				const Transform *const transform = transforms + index;
				Collision *const collision = collisions + index;

				const float h_size = 16.f;

				BoundingBox global_aabb;
				global_aabb.center.x = transform->position.x + h_size;
				global_aabb.center.y = transform->position.y + h_size;
				global_aabb.halfSize = { h_size, h_size };

				collision->boundary = !CheckAabbInside(
					screenAabb,
					global_aabb);
			}
		}
	}

	void DetectCollisions()
	{
		World *const world = g_context->world;

		for (const auto collisionMask : world->collisionMasks)
		{
			Layer *const first_layer = ResolveLayer(collisionMask.first);
			Layer *const second_layer = ResolveLayer(collisionMask.second);

			bool broad_collision = CheckAabbCollision(
				first_layer->boundingBox,
				second_layer->boundingBox);
			if (broad_collision == false)
				continue;

			const uint16_t first_count = ResolveModelCount(*first_layer);
			const uint16_t second_count = ResolveModelCount(*second_layer);

			const uint16_t *const first_indexes = ResolveModelIndexes(*first_layer);
			const uint16_t *const second_indexes = ResolveModelIndexes(*second_layer);

			const Transform *const first_transforms = ResolveModelTransformData(*first_layer);
			const Transform *const second_transforms = ResolveModelTransformData(*second_layer);

			const Collider *const first_colliders = ResolveModelColliderData(*first_layer);
			const Collider *const second_colliders = ResolveModelColliderData(*second_layer);

			Collision *const first_collisions = ResolveModelCollisionData(*first_layer);
			Collision *const second_collisions = ResolveModelCollisionData(*second_layer);

			for (uint16_t i = 0; i < first_count; ++i)
			{
				const uint16_t first_index = first_indexes[i];
				const Transform *const first_transform = first_transforms + first_index;
				const Collider *const first_collider = first_colliders + first_index;
				Collision *const first_collision = first_collisions + first_index;

				BoundingBox first_global_aabb;
				first_global_aabb.center.x = first_transform->position.x + first_collider->localBb.center.x;
				first_global_aabb.center.y = first_transform->position.y + first_collider->localBb.center.y;
				first_global_aabb.halfSize = first_collider->localBb.halfSize;

				for (uint16_t j = 0; j < second_count; ++j)
				{
					const uint16_t second_index = second_indexes[j];
					const Transform *const second_transform = second_transforms + second_index;
					const Collider *const second_collider = second_colliders + second_index;
					Collision *const second_collision = second_collisions + second_index;

					BoundingBox second_global_aabb;
					second_global_aabb.center.x = second_transform->position.x + second_collider->localBb.center.x;
					second_global_aabb.center.y = second_transform->position.y + second_collider->localBb.center.y;
					second_global_aabb.halfSize = second_collider->localBb.halfSize;

					const bool collided = CheckAabbCollision(first_global_aabb, second_global_aabb);

					if (collided)
					{
						first_collision->collidedLayers |= 1 << collisionMask.second;
						second_collision->collidedLayers |= 1 << collisionMask.first;
					}
				}
			}
		}
	}

	// Check wheter b is inside a
	inline bool CheckAabbInside(const BoundingBox& b1, const BoundingBox& b2)
	{
		if (abs(b1.center.x - b2.center.x) > b1.halfSize.x - b2.halfSize.x) return false;
		if (abs(b1.center.y - b2.center.y) > b1.halfSize.y - b2.halfSize.y) return false;
		return true;
	}

	inline bool CheckAabbCollision(const Vector2& p1, const Vector2& p2, const BoundingBox& b1, const BoundingBox& b2)
	{
		if (abs(p1.x + b1.center.x - p2.x + b2.center.x) > b1.halfSize.x + b2.halfSize.x) return false;
		if (abs(p1.y + b1.center.y - p2.y + b2.center.y) > b1.halfSize.y + b2.halfSize.y) return false;
		return true;
	}

	inline bool CheckAabbCollision(const BoundingBox& b1, const BoundingBox& b2)
	{
		if (abs(b1.center.x - b2.center.x) > b1.halfSize.x + b2.halfSize.x) return false;
		if (abs(b1.center.y - b2.center.y) > b1.halfSize.y + b2.halfSize.y) return false;
		return true;
	}

	inline bool CheckAabbCollision(
		const float x1, const float y1, const float w1, const float h1,
		const float x2, const float y2, const float w2, const float h2)
	{
		if (abs(x1 - x2) > w1 + w2) return false;
		if (abs(y1 - y2) > h1 + h2) return false;
		return true;
	}
}
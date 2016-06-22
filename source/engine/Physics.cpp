#include "Engine.h"

namespace Engine
{
	static void UpdateLayerBoundingBox(
		const uint16_t *const indexes,
		const Transform *const transforms,
		const uint16_t count,
		BoundingBox *const boundingBox);

	static void ResetCollisions(
		const uint16_t *const indexes,
		const uint16_t count,
		Collision *const collisions);

	static void DetectBoundaryCollisions(
		const BoundingBox& boundingBox,
		const uint16_t *const indexes,
		const Transform *const transforms,
		const uint16_t count,
		Collision *const collisions);

	static void DetectCollisions(
		const BoundingBox& first_bounding_box,
		const BoundingBox& second_bounding_box,
		const LayerId first_layer_id,
		const LayerId second_layer_id,
		const uint16_t *const first_indexes,
		const uint16_t *const second_indexes,
		const Transform *const first_transforms,
		const Transform *const second_transforms,
		const Collider *const first_colliders,
		const Collider *const second_colliders,
		const uint16_t first_count,
		const uint16_t second_count,
		Collision *const first_collisions,
		Collision *const second_collisions);

	// Check wheter b is inside a
	inline bool CheckAabbInside(const BoundingBox& b1, const BoundingBox& b2);
	inline bool CheckAabbCollision(const Vector2& p1, const Vector2& p2, const BoundingBox& b1, const BoundingBox& b2);
	inline bool CheckAabbCollision(const BoundingBox& b1, const BoundingBox& b2);
	inline bool CheckAabbCollision(
		const float x1, const float y1, const float w1, const float h1,
		const float x2, const float y2, const float w2, const float h2);

	void UpdatePhysicsBroadPhase(ComponentManager::LayerData *const data)
	{
		const uint16_t count = data->components.Size();
		if (count == 0)
			return;

		const uint16_t *const indexes = data->components.Indexes();
		const Transform *const transforms = data->components.Data<Transform>(MODEL_DATA_TRANSFORM);

		BoundingBox *const bounding_box = &GetLayerData(data->layerId)->boundingBox;

		UpdateLayerBoundingBox(
			indexes,
			transforms,
			count,
			bounding_box);

		Collision *const collisions = data->components.Data<Collision>(MODEL_DATA_COLLISION);
		ResetCollisions(
			indexes,
			count,
			collisions);
	}

	void UpdatePhysicsNarrowPhase(ComponentManager::LayerData *const data)
	{
		const uint16_t count = data->components.Size();
		if (count == 0)
			return;

		const uint16_t *const indexes = data->components.Indexes();
		const Transform *const transforms = data->components.Data<Transform>(MODEL_DATA_TRANSFORM);
		Collision *const collisions = data->components.Data<Collision>(MODEL_DATA_COLLISION);
		const Collider *const colliders = data->components.Data<Collider>(MODEL_DATA_COLLIDER);

		const BoundingBox *const bounding_box = &GetLayerData(data->layerId)->boundingBox;

		DetectBoundaryCollisions(
			*bounding_box,
			indexes,
			transforms,
			count,
			collisions);

		for (const auto mask : g_context->world->collisionMasks)
		{
			if (mask.first != data->layerId)
				continue;

			const ComponentManager::LayerData *const other_data = &GetComponentManager(COMPONENT_TYPE_MODEL)->layerData[mask.second];

			const uint16_t other_count = other_data->components.Size();
			if (other_count == 0)
				continue;

			const BoundingBox *const other_bounding_box = &GetLayerData(other_data->layerId)->boundingBox;

			const bool broadCollision = CheckAabbCollision(*bounding_box, *other_bounding_box);
			if (broadCollision == false)
				continue;

			const uint16_t *const other_indexes = other_data->components.Indexes();
			const Transform *const other_transforms = other_data->components.Data<Transform>(MODEL_DATA_TRANSFORM);
			Collision *const other_collisions = other_data->components.Data<Collision>(MODEL_DATA_COLLISION);
			const Collider *const other_colliders = other_data->components.Data<Collider>(MODEL_DATA_COLLIDER);

			DetectCollisions(
				*bounding_box,
				*other_bounding_box,
				data->layerId,
				other_data->layerId,
				indexes,
				other_indexes,
				transforms,
				other_transforms,
				colliders,
				other_colliders,
				count,
				other_count,
				collisions,
				other_collisions);
		}
	}

	static void UpdateLayerBoundingBox(
		const uint16_t *const indexes,
		const Transform *const transforms,
		const uint16_t count,
		BoundingBox *const boundingBox)
	{
		if (count == 0)
		{
			boundingBox->center = { 0.f, 0.f };
			boundingBox->halfSize = { 0.f, 0.f };

			return;
		}

		const float screen_width = static_cast<float>(ScreenWidth());
		const float screen_height = static_cast<float>(ScreenHeight());
		const float size = 32.f;

		float min_x = screen_width;
		float max_x = 0.f;
		float min_y = screen_height;
		float max_y = 0.f;

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

		boundingBox->center.x = (min_x + max_x) / 2.f;
		boundingBox->center.y = (min_y + max_y) / 2.f;
		boundingBox->halfSize.x = (max_x - min_x) / 2.f;
		boundingBox->halfSize.y = (max_y - min_y) / 2.f;
	}

	static void ResetCollisions(
		const uint16_t *const indexes,
		const uint16_t count,
		Collision *const collisions)
	{
		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			Collision *const collision = collisions + index;

			collision->collidedLayers = 0;
			collision->boundary = 0;

		}
	}

	static void DetectBoundaryCollisions(
		const BoundingBox& boundingBox,
		const uint16_t *const indexes,
		const Transform *const transforms,
		const uint16_t count,
		Collision *const collisions)
	{
		const float h_screen_width = ScreenWidth() / 2.f;
		const float h_screen_height = ScreenHeight() / 2.f;

		BoundingBox screenAabb;
		screenAabb.center = { h_screen_width, h_screen_height };
		screenAabb.halfSize = { h_screen_width, h_screen_height };

		bool broad_inside = CheckAabbInside(
			screenAabb,
			boundingBox);
		if (broad_inside == true)
			return;

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

			collision->boundary = !CheckAabbCollision(
				screenAabb,
				global_aabb);

		}
	}

	static void DetectCollisions(
		const BoundingBox& first_bounding_box,
		const BoundingBox& second_bounding_box,
		const LayerId first_layer_id,
		const LayerId second_layer_id,
		const uint16_t *const first_indexes,
		const uint16_t *const second_indexes,
		const Transform *const first_transforms,
		const Transform *const second_transforms,
		const Collider *const first_colliders,
		const Collider *const second_colliders,
		const uint16_t first_count,
		const uint16_t second_count,
		Collision *const first_collisions,
		Collision *const second_collisions)
	{
		bool broad_collision = CheckAabbCollision(
			first_bounding_box,
			second_bounding_box);
		if (broad_collision == false)
			return;

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
					first_collision->collidedLayers |= 1 << second_layer_id;
					second_collision->collidedLayers |= 1 << first_layer_id;
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
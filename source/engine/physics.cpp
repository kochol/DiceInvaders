/* ---------------------------------------------------------------------------
**
** physics.cpp
** Collision detection stuff, implemented as a part of "Model" component
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#include "engine.h"

#include <algorithm>

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
	
	// Check wheter a collides with b
	inline bool CheckAabbCollision(const Vector2& p1, const Vector2& p2, const BoundingBox& b1, const BoundingBox& b2);
	inline bool CheckAabbCollision(const BoundingBox& b1, const BoundingBox& b2);
	inline bool CheckAabbCollision(
		const float x1, const float y1, const float w1, const float h1,
		const float x2, const float y2, const float w2, const float h2);


	// The broad-phase is impleneted by checking each layer's bounding box against other layers
	// This simple splution was selected because of the good-enough results achieved for the problem.
	void UpdatePhysicsBroadPhase(ComponentManager::LayerData *const data)
	{
		const uint16_t count = data->components.Size();
		if (count == 0)
			return;

		const uint16_t *const indexes = data->components.Indexes();
		const Transform *const transforms = data->components.Data<Transform>(MODEL_DATA_TRANSFORM);

		BoundingBox *const bounding_box = &GetLayerData(data->layer_id)->aabb;

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

	// The narrow-phase checks each objects of a layer againts each object of the 
	// other layer. It is of O(n2) complexity, sor for heavier scenarios, use of
	// some sort of spatial partitioning and stricter broad-phase would be necessary.
	void UpdatePhysicsNarrowPhase(ComponentManager::LayerData *const data)
	{
		const uint16_t count = data->components.Size();
		if (count == 0)
			return;

		const uint16_t *const indexes = data->components.Indexes();
		const Transform *const transforms = data->components.Data<Transform>(MODEL_DATA_TRANSFORM);
		Collision *const collisions = data->components.Data<Collision>(MODEL_DATA_COLLISION);
		const Collider *const colliders = data->components.Data<Collider>(MODEL_DATA_COLLIDER);

		const BoundingBox *const bounding_box = &GetLayerData(data->layer_id)->aabb;

		DetectBoundaryCollisions(
			*bounding_box,
			indexes,
			transforms,
			count,
			collisions);

		for (const auto mask : g_context->world->collision_masks)
		{
			if (mask.first != data->layer_id)
				continue;

			const ComponentManager::LayerData *const other_data = &GetComponentManager(COMPONENT_TYPE_MODEL)->layer_data[mask.second];

			const uint16_t other_count = other_data->components.Size();
			if (other_count == 0)
				continue;

			const BoundingBox *const other_bounding_box = &GetLayerData(other_data->layer_id)->aabb;

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
				data->layer_id,
				other_data->layer_id,
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

	// Updates aabb of a layer, which is then used in the broad-phase
	// for early layer-layer rejection
	static void UpdateLayerBoundingBox(
		const uint16_t *const indexes,
		const Transform *const transforms,
		const uint16_t count,
		BoundingBox *const boundingBox)
	{
		if (count == 0)
		{
			boundingBox->center = { 0.f, 0.f };
			boundingBox->half_size = { 0.f, 0.f };

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

			min_x = std::min(min_x, x);
			max_x = std::max(max_x, x);
			min_y = std::min(min_y, y);
			max_y = std::max(max_y, y);
		}

		max_x += size;
		max_y += size;

		boundingBox->center.x = (min_x + max_x) / 2.f;
		boundingBox->center.y = (min_y + max_y) / 2.f;
		boundingBox->half_size.x = (max_x - min_x) / 2.f;
		boundingBox->half_size.y = (max_y - min_y) / 2.f;
	}

	// Resets the state of the collision output data structures
	static void ResetCollisions(
		const uint16_t *const indexes,
		const uint16_t count,
		Collision *const collisions)
	{
		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			Collision *const collision = collisions + index;

			collision->collided_layers = 0;
			collision->boundary = 0;

		}
	}

	// Detects if an object is out of screen boundaries
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
		screenAabb.half_size = { h_screen_width, h_screen_height };

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
			global_aabb.half_size = { h_size, h_size };

			collision->boundary = !CheckAabbCollision(
				screenAabb,
				global_aabb);

		}
	}

	// Detect collision between two layers
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
			first_global_aabb.center.x = first_transform->position.x + first_collider->local_aabb.center.x;
			first_global_aabb.center.y = first_transform->position.y + first_collider->local_aabb.center.y;
			first_global_aabb.half_size = first_collider->local_aabb.half_size;

			for (uint16_t j = 0; j < second_count; ++j)
			{
				const uint16_t second_index = second_indexes[j];
				const Transform *const second_transform = second_transforms + second_index;
				const Collider *const second_collider = second_colliders + second_index;
				Collision *const second_collision = second_collisions + second_index;

				BoundingBox second_global_aabb;
				second_global_aabb.center.x = second_transform->position.x + second_collider->local_aabb.center.x;
				second_global_aabb.center.y = second_transform->position.y + second_collider->local_aabb.center.y;
				second_global_aabb.half_size = second_collider->local_aabb.half_size;

				const bool collided = CheckAabbCollision(first_global_aabb, second_global_aabb);

				if (collided)
				{
					first_collision->collided_layers |= 1 << second_layer_id;
					second_collision->collided_layers |= 1 << first_layer_id;
				}
			}
		}
	}

	// Check wheter b is inside a
	inline bool CheckAabbInside(const BoundingBox& b1, const BoundingBox& b2)
	{
		if (abs(b1.center.x - b2.center.x) > b1.half_size.x - b2.half_size.x) return false;
		if (abs(b1.center.y - b2.center.y) > b1.half_size.y - b2.half_size.y) return false;
		return true;
	}

	// Check wheter a collides with b
	inline bool CheckAabbCollision(const Vector2& p1, const Vector2& p2, const BoundingBox& b1, const BoundingBox& b2)
	{
		if (abs(p1.x + b1.center.x - p2.x + b2.center.x) > b1.half_size.x + b2.half_size.x) return false;
		if (abs(p1.y + b1.center.y - p2.y + b2.center.y) > b1.half_size.y + b2.half_size.y) return false;
		return true;
	}

	inline bool CheckAabbCollision(const BoundingBox& b1, const BoundingBox& b2)
	{
		if (abs(b1.center.x - b2.center.x) > b1.half_size.x + b2.half_size.x) return false;
		if (abs(b1.center.y - b2.center.y) > b1.half_size.y + b2.half_size.y) return false;
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

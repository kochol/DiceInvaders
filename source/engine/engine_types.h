/* ---------------------------------------------------------------------------
**
** engine_types.h
** Engine type definitions
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once
#include <unordered_set>

#include "core/library.h"
#include "core/entity_manager.h"
#include "core/roster_pool.h"
#include "core/enums.h"

namespace Engine
{
	struct Vector2
	{
		float x;
		float y;
	};

	struct BoundingBox
	{
		Vector2 center;
		Vector2 half_size;
	};

	struct ComponentManager
	{
		ComponentType type;

		// maps and entity handle to a component handle of this type, in this layer
		HandleHashMap<EntityHandle, ComponentHandle> component_map;

		struct LayerData
		{
			LayerId layer_id;
			RosterPool components;
			std::unordered_set<uint16_t> to_be_freed;

			// custom data that is managed by the manager
			void *custom_data;
		};
		LayerData layer_data[LAYER_ID_MAX];

		typedef void(*Callback)(LayerData *const);
		Callback callbacks[CALLBACK_STAGE_MAX];
	};

	struct Resources
	{
		// store all loaded resources
		RosterPool caches[RESOURCE_TYPE_MAX];
		HandleHashSet<ResourceHandle> to_be_freed;
	};

	
	struct BaseComponent
	{
		// handle to itself
		ComponentHandle self;
		// handle to the entity which the component is attached to
		EntityHandle entity;
		// cache handle to the model component
		ComponentHandle model;
	};

	struct Transform
	{
		Vector2 position;
	};

	struct Collider
	{
		BoundingBox local_aabb;
	};

	// collision detection results
	struct alignas(4) Collision
	{
		uint16_t collided_layers;
		uint8_t boundary;
		uint8_t padding;
	};

	struct World
	{
		EntityManager handle_manager;
		HandleHashSet<EntityHandle> to_be_freed_entities;

		struct LayerData
		{
			LayerId layer_id;
			BoundingBox aabb;
			uint16_t max_entities;
		};

		LayerData layers[LAYER_ID_MAX];
		ComponentManager components[COMPONENT_TYPE_MAX];

		std::vector<std::pair<LayerId, LayerId>> collision_masks;
	};

	// data specific to a frame, used by components
	struct FrameData
	{
		float dt;
		float time;
		KeyStatus keys;
	};

	struct Config
	{
		int screen_width;
		int screen_height;
	};


	// Engine context
	struct Context
	{
		World *world;
		Resources *resources;
		ILibrary *system;

		Config *config;

		FrameData *frame_data;
	};
}

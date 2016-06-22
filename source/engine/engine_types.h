#pragma once
#include <unordered_set>

#include "core/Library.h"
#include "core/entity_manager.h"
#include "core/roster_pool.h"
#include "core/Enums.h"

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

		// these types are so rarely used
		// use entity id as the key
		HandleHashMap<EntityHandle, ComponentHandle> component_map;

		struct LayerData
		{
			LayerId layer_id;
			RosterPool components;
			std::unordered_set<uint16_t> to_be_freed;
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
		ComponentHandle self;
		EntityHandle entity;
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

	struct alignas(4) Collision
	{
		uint16_t collided_layers;
		uint8_t boundary;
		uint8_t padding;
	};

	struct World
	{
		// uniformly indexed common components.
		// component id can be used as a direct index on them
		EntityHandleManager handle_manager;
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

	struct Context
	{
		World *world;
		Resources *resources;
		ILibrary *system;

		Config *config;

		FrameData *frame_data;
	};
}

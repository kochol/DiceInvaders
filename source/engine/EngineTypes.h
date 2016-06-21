#pragma once
#include <unordered_set>

#include "core/Library.h"
#include "core/EntityHandleManager.h"
#include "core/RosterPool.h"
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
		Vector2 halfSize;
	};

	struct ComponentManager
	{
		ComponentType type;

		// these types are so rarely used
		// use entity id as the key
		HandleHashMap<EntityHandle, ComponentHandle> componentMap;

		struct LayerData
		{
			LayerId layerId;
			RosterPool components;
			std::unordered_set<uint16_t> toBeFreed;
			void *customData;
		};
		LayerData layerData[LAYER_ID_MAX];

		typedef void(*Callback)(LayerData *const);
		Callback callbacks[CALLBACK_STAGE_MAX];
	};

	struct Resources
	{
		// store all loaded resources
		RosterPool caches[RESOURCE_TYPE_MAX];
		HandleHashSet<ResourceHandle> toBeFreed;
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
		BoundingBox localBb;
	};

	struct alignas(4) Collision
	{
		uint16_t collidedLayers;
		uint8_t boundary;
		uint8_t padding;
	};

	struct World
	{
		// uniformly indexed common components.
		// component id can be used as a direct index on them
		EntityHandleManager handleManager;
		HandleHashSet<EntityHandle> toBeFreedEntities;

		struct LayerData
		{
			LayerId layerId;
			BoundingBox boundingBox;
			uint16_t maxEntities;
		};

		LayerData layers[LAYER_ID_MAX];
		ComponentManager components[COMPONENT_TYPE_MAX];

		std::vector<std::pair<LayerId, LayerId>> collisionMasks;
		
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

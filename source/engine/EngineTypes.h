#pragma once
#include "core/Library.h"
#include <unordered_map>
#include "core/EntityHandleManager.h"
#include "core/RosterPool.h"

namespace Engine
{
	struct BaseComponent
	{
		EntityHandle entity;
		ComponentHandle model;
	};

	struct ComponentManager
	{
		ComponentType type;
		// these types are so rarely used
		// use entity id as the key
		HandleHashMap<EntityHandle, ComponentHandle> componentMap;
		RosterPool components;

		typedef void(*Callback)(ComponentManager *const);
		Callback callbacks[CALLBACK_STAGE_MAX];

		void *customData;
	};

	struct Resources
	{
		// store all loaded resources
		RosterPool caches[RESOURCE_TYPE_MAX];
	};

	struct Transform
	{
		struct
		{
			float x;
			float y;
		} position;
	};

	struct Collider
	{
		struct
		{
			float left;
			float down;
			float right;
			float top;
		} localBb;
	};

	struct Collision
	{
		LayerId collidedLayer;
		union
		{
			struct
			{
				bool top : 2;
				bool left : 2;
				bool right : 2;
				bool down : 2;
			};
			uint8_t any;
		} out;
	};

	struct Layer
	{
		LayerId id;
		RosterPool models;
	};

	struct World
	{
		// uniformly indexed common components.
		// component id can be used as a direct index on them
		EntityHandleManager handleManager;
		HandleHashMap<EntityHandle, ComponentHandle> modelMap;

		Layer *layers[LAYER_ID_MAX];

		ComponentManager *components[COMPONENT_TYPE_MAX];

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

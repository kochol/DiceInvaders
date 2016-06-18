#pragma once
#include "core/Library.h"
#include "core/EntityHandleManager.h"
#include "core/RosterPool.h"
#include "core/Enums.h"

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

	struct BoundingBox
	{
		float x;
		float w;
		float y;
		float h;
	};

	struct Point
	{
		float x;
		float y;
	};

	struct Transform
	{
		Point position;
	};

	struct Collider
	{
		BoundingBox localBb;
	};

	struct Collision
	{
		LayerId collidedLayer;
		bool boundary;
	};

	struct Layer
	{
		LayerId id;
		RosterPool models;
		BoundingBox boundingBox;
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

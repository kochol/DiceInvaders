#pragma once
#include "core/Library.h"
#include <unordered_map>
#include <unordered_set>
#include "core/EntityHandleManager.h"
#include "core/RosterPool.h"

namespace Engine
{
	struct Transform
	{
		EntityHandle entity;
		float x;
		float y;
	};

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
		std::unordered_map<CallbackStage, Callback> callbacks;

		void *customData;
	};

	struct Rocket : BaseComponent
	{
	};

	struct Bomb : BaseComponent
	{
	};

	struct CollisionInfo
	{
		EntityHandle first;
		EntityHandle second;
	};

	struct Resources
	{
		// store all loaded sprites
		RosterPool spriteCache;
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

		std::unordered_map<LayerId, Layer*> layers;

		std::unordered_map<ComponentType, ComponentManager*> components;

		std::vector<std::pair<LayerId, LayerId>> collisionMasks;
	};

	struct FrameData
	{
		float dt;
		float time;
		KeyStatus keys;

		std::unordered_map<LayerId, std::vector<EntityHandle>> collisions;
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

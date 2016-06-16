#pragma once
#include <cstdint>
#include "Library.h"
#include <unordered_map>
#include <map>
#include "EntityHandleManager.h"
#include "RosterPool.h"


namespace Engine
{
	struct Transform
	{
		EntityHandle handle;
		float x;
		float y;
	};

	struct BaseComponent
	{
		EntityHandle handle;
		ComponentHandle transform;
	};

	struct ComponentManager
	{
		ComponentType type;
		// these types are so rarely used
		// use entity id as the key
		HandleHashMap<EntityHandle, ComponentHandle> componentMap;
		RosterPool components;
	};

	struct Rocket : BaseComponent
	{
	};

	struct Alien : BaseComponent
	{
		float lastDroped;
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
		RosterPoolTyped<ResourceHandle> handleManager;
		// store all loaded sprites
		HandleHashMap<ResourceHandle, ISprite*> spriteMap;
	};

	struct Layer
	{
		LayerId id;
		RosterPoolTyped<Transform> transforms;
		RosterPoolTyped<ResourceHandle> sprites;
	};

	struct World
	{
		// uniformly indexed common components.
		// component id can be used as a direct index on them
		EntityHandleManager handleManager;
		HandleHashMap<EntityHandle, ComponentHandle> modelMap;

		std::map<LayerId, Layer*> layers;

		std::vector<ComponentManager> components;

		std::vector<std::pair<LayerId, LayerId>> collisionMasks;
	};

	struct FrameData
	{
		float dt;
		float time;
		KeyStatus keys;

		std::vector<CollisionInfo> collisions;
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

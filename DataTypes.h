#pragma once
#include <cstdint>
#include "DiceInvaders.h"
#include <unordered_map>
#include "HandleManager.h"
#include "RosteredPool.h"

namespace Engine
{
	typedef Handle EntityHandle;
	typedef Handle ComponentHandle;
	typedef Handle ResourceHandle;

	typedef uint8_t Layer;

	struct Sprite
	{
		ISprite *sprite;
	};

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
		// these types are so rarely used
		// use entity id as the key
		HandleHashMap<ComponentHandle> componentMap;
		RosteredPool components;
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
		HandleManager handleManager;
		// store all loaded sprites
		HandleHashMap<Sprite> spriteMap;
	};

	struct World
	{
		// uniformly indexed common components.
		// component id can be used as a direct index on them
		HandleManager handleManager;
		HandleHashMap<ComponentHandle> modelMap;
		std::vector<Transform> transforms;
		std::vector<Sprite> sprites;
		std::vector<Layer> layers;

		std::vector<ComponentManager> components;
	};

	struct Context
	{
		World *world;
		Resources *resources;
		IDiceInvaders *system;
		int width, height;
	};
}

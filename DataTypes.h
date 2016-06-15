#pragma once
#include <cstdint>
#include "DiceInvaders.h"
#include <unordered_map>
#include "EntityManager.h"

namespace DiceInvaders
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

	struct Player : BaseComponent
	{
		float lastFired;
		uint8_t health;
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
		HandleHashMap<ComponentHandle> entityMap;
		std::vector<Transform> transforms;
		std::vector<Sprite> sprites;
		std::vector<Layer> layers;

		// these types are so rarely used
		// use entity id as the key
		HandleHashMap<ComponentHandle> playerMap;
		HandleHashMap<ComponentHandle> rocketMap;
		HandleHashMap<ComponentHandle> alienMap;
		HandleHashMap<ComponentHandle> bombMap;

		std::vector<Player> players;
		std::vector<Rocket> rockets;
		std::vector<Alien> aliens;
		std::vector<Bomb> bombs;
	};

	struct Context
	{
		World *world;
		Resources *resources;
		IDiceInvaders *system;
	};
}
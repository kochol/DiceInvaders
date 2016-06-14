#pragma once
#include <cstdint>
#include "RosteredPool.h"
#include <string>
#include <array>

namespace ComponentSystem
{
	typedef uint16_t Handle;
	typedef uint16_t Type;
	typedef uint32_t* Chain;

	struct ComponentType
	{
		std::string Name;
		Type Id;
		RosteredPool Pool;
	};

	struct Component
	{
		Handle Id;
		Handle HostId;
	};

	struct GameObject
	{
		Handle Id;
		Chain ComponentChain;
	};

	class ComponentManager
	{
	private:
		RosteredPool go_pool;
		RosteredPool chain_pool;

		ComponentType types[10];

	public:

		ComponentManager();

		Type RegisterComponentType(const std::string name_str, const uint16_t max_count, const size_t item_size);

		GameObject*    CreateGameObject();
		GameObject*    ResolveGameObjectHandle(const Handle handle);
		void           DestroyGameObject(const Handle handle);

		Handle     CreateComponent(const Type type, const Handle host);
		Component*     ResolveComponentHandle(const Type type, const Handle handle);
		Handle         GetComponentOfType(const Type type, const Handle host);
		void DestroyComponent(const Type type, const Handle handle);
		void DestroyComponentChain(const Type type, Chain* chain);
	};
}

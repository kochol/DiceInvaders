#include "ComponentSystem.h"
namespace ComponentSystem
{
	ComponentManager::ComponentManager()
	{
		go_pool.Init(1000, sizeof(GameObject));
		chain_pool.Init(5000, sizeof(uint32_t));
	}

	Type ComponentManager::RegisterComponentType(const std::string name_str, const uint16_t max_count, const size_t item_size)
	{
		
	}
}

#include "entity_manager.h"
#include <cassert>

namespace Engine
{
	EntityHandle EntityHandleManager::Create(const LayerId layer)
	{
		EntityHandle handle;
		if (_free_ids.size() > 1024)
		{
			handle.index = _free_ids.front();
			_free_ids.pop_front();
		}
		else
		{
			handle.index = static_cast<uint16_t>(_salts.size());
			_salts.push_back(0);
		}

		handle.header.salt = _salts[handle.index];
		handle.header.layer = static_cast<uint8_t>(layer);

		return handle;
	}

	bool EntityHandleManager::Valid(const EntityHandle handle) const
	{
		assert(handle.index < _salts.size());
		return handle.header.salt == _salts[handle.index];
	}

	void EntityHandleManager::Destroy(const EntityHandle handle)
	{
		assert(Valid(handle));

		assert(handle.header.salt < UINT8_MAX);
		++_salts[handle.index];

		_free_ids.push_back(handle.index);
	}
}

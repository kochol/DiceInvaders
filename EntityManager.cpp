#include "EntityManager.h"
#include <cassert>
#include "DataTypes.h"

namespace DiceInvaders
{
	Handle HandleManager::Create()
	{
		Handle handle;
		if (_free_ids.size() > 0)
		{
			handle = _free_ids.front();
			_free_ids.pop_front();
			++handle.salt;
		}
		else
		{
			_salts.push_back(0);
			handle.index = _salts.size() - 1;
			handle.salt = 0;
		}

		return handle;
	}

	bool HandleManager::Valid(const Handle handle) const
	{
		assert(handle.index < _salts.size());
		return handle.salt == _salts[handle.index];
	}

	void HandleManager::Destroy(const Handle handle)
	{
		assert(Valid(handle));

		assert(handle.salt < UINT8_MAX);
		++_salts[handle.index];

		_free_ids.push_back(handle);
	}
}

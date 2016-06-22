/* ---------------------------------------------------------------------------
**
** entity_manager.cpp
** Entity handle manager, based on Bitsquid's implementation
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#include "entity_manager.h"

#include <cassert>

/* -- module -----------------------------------------------------------------
**
** The entity manager is responsibe for creating and destroying unique handles.
** The handles are used in the engine instead of raw pointers. The manager is
** capable of very fast handle validation.
**
/* -- implementation ---------------------------------------------------------
**
** Internally the manager creates new handles in a linear fashion, only using 
** the previusly destroyed handles when there are too many (in this rough 
** implementation, hardcoded as 1024) freed handles in the queue.
** Validity check is implemented by using a per-handle salt value that inceases 
** each time the handle is used. The backing type of the salt value is uint8_t,
** so each handle can be reused 255 times, which is far from neccessary in this
** simple game :D
**
** -------------------------------------------------------------------------*/

namespace Engine
{
	EntityHandle EntityManager::Create(const LayerId layer)
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

	bool EntityManager::Valid(const EntityHandle handle) const
	{
		assert(handle.index < _salts.size());
		return handle.header.salt == _salts[handle.index];
	}

	void EntityManager::Destroy(const EntityHandle handle)
	{
		assert(Valid(handle));

		assert(handle.header.salt < UINT8_MAX);
		++_salts[handle.index];

		_free_ids.push_back(handle.index);
	}
}

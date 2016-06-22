/* ---------------------------------------------------------------------------
**
** entity_manager.h
** Entity handle manager, based on Bitsquid's implementation
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once
#include <deque>
#include <vector>

#include "handle.h"
#include "enums.h"

namespace Engine
{
	class EntityManager
	{
	private:
		std::deque<uint16_t> _free_ids;
		std::vector<uint8_t> _salts;

	public:
		// Creates a new entity in the specified layer.
		// It does not allocate any data, just creates a new handle
		EntityHandle Create(const LayerId layer);

		// Check whether the handle is valid
		bool Valid(const EntityHandle handle) const;

		// destroys the handle, making it availible for future creations
		void Destroy(const EntityHandle handle);
	};
}

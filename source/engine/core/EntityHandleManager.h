#pragma once
#include <deque>
#include <vector>

#include "Handle.h"

namespace Engine
{
	class EntityHandleManager
	{
	private:
		std::deque<uint16_t> _free_ids;
		std::vector<uint8_t> _salts;

	public:
		EntityHandle Create(const LayerId layer);
		bool Valid(const EntityHandle handle) const;
		void Destroy(const EntityHandle handle);
	};
}

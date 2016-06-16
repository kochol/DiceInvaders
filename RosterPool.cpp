#include "RosterPool.h"
#include <cstdlib>
#include <cassert>
#include <utility>

namespace Engine
{
	RosterPool::RosterPool() :
		buffer(nullptr),
		item_size(0),
		max_item_count(0),
		item_partition(0),
		indexes(nullptr)
	{}

	void RosterPool::Init(const uint16_t _max_item_count, const size_t _item_size)
	{
		item_size = _item_size;
		max_item_count = _max_item_count;
		item_partition = 0;

		buffer = reinterpret_cast<uint8_t*>(malloc(_max_item_count * item_size));
		assert(buffer != nullptr);
		indexes = reinterpret_cast<uint16_t*>(malloc(_max_item_count * sizeof(uint16_t)));
		assert(indexes != nullptr);
		for (uint16_t i = 0; i < max_item_count; ++i)
			indexes[i] = i;
	}

	RosterPool::~RosterPool()
	{
		free(buffer);
		free(indexes);
	}

	uint16_t RosterPool::Alloc()
	{
		assert(item_partition < max_item_count);
		return indexes[item_partition++];
	}

	void RosterPool::Free(const uint16_t handle)
	{
		assert(handle < max_item_count);
		std::swap(indexes[item_partition - 1], indexes[handle]);
		--item_partition;
	}
}

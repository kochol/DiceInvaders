#include "RosterPool.h"
#include <cstdlib>
#include <cassert>
#include <utility>

namespace Engine
{
	RosterPool::RosterPool() :
		buffer(nullptr),
		buffer_count(0),
		item_size(nullptr),
		max_item_count(0),
		item_partition(0),
		indexes(nullptr)
	{}

	void RosterPool::Init(const uint16_t _max_item_count, const size_t _item_size)
	{
		Init(1, _max_item_count, &_item_size);
	}

	void RosterPool::Init(const uint8_t _buffer_count, const uint16_t _max_item_count, const size_t *const _item_size)
	{
		buffer_count = _buffer_count;
		item_size = new size_t[_buffer_count];
		buffer = new uint8_t*[_buffer_count];
		for (uint8_t i = 0; i < buffer_count; ++i)
		{
			item_size[i] = _item_size[i];
			buffer[i] = reinterpret_cast<uint8_t*>(malloc(_max_item_count * item_size[i]));
		}

		max_item_count = _max_item_count;
		item_partition = 0;
		
		indexes = reinterpret_cast<uint16_t*>(malloc(_max_item_count * sizeof(uint16_t)));
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

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
		indexes(nullptr),
		rev_indexes(nullptr)
	{}

	void RosterPool::Init(const uint16_t _max_item_count, std::initializer_list<size_t> _item_size)
	{
		buffer_count = _item_size.size();

		item_size = new size_t[buffer_count];
		buffer = new uint8_t*[buffer_count];
		for (uint8_t i = 0; i < buffer_count; ++i)
		{
			item_size[i] = _item_size.begin()[i];
			buffer[i] = reinterpret_cast<uint8_t*>(malloc(_max_item_count * item_size[i]));
		}

		max_item_count = _max_item_count;
		item_partition = 0;
		
		indexes = reinterpret_cast<uint16_t*>(malloc(_max_item_count * sizeof(uint16_t)));
		rev_indexes = reinterpret_cast<uint16_t*>(malloc(_max_item_count * sizeof(uint16_t)));
		for (uint16_t i = 0; i < max_item_count; ++i)
		{
			indexes[i] = i;
			rev_indexes[i] = i;
		}
	}

	RosterPool::~RosterPool()
	{
		for (uint8_t i = 0; i < buffer_count; ++i)
			free(buffer[i]);

		delete buffer;
		delete item_size;

		free(indexes);
		free(rev_indexes);
	}

	uint16_t RosterPool::Alloc()
	{
		assert(item_partition < max_item_count);
		return indexes[item_partition++];
	}

	void RosterPool::Free(const uint16_t handle)
	{
		assert(handle < max_item_count);

		const uint16_t free_index = rev_indexes[handle];
		const uint16_t move_index = item_partition - 1;

		const uint16_t free_handle = handle;
		const uint16_t move_handle = indexes[item_partition - 1];

		assert(free_index < item_partition);

		std::swap(indexes[free_index], indexes[move_index]);
		std::swap(rev_indexes[free_handle], rev_indexes[move_handle]);
		--item_partition;
	}
}

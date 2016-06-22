/* ---------------------------------------------------------------------------
**
** roster_pool.cpp
** Multi-buffer memory pool, based on Insomniac's implementation, allowing
** very fast free/alloc operations
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#include "roster_pool.h"

#include <cstdlib>
#include <cassert>
#include <utility>

/* -- module -----------------------------------------------------------------
**
** RosterPool is a multi-buffer fixed-size memory pool implementation, 
** that allows very fast alloc and free operations. The pool uses the
** Structure-of-Arrays idea, so each pool entry points to n locations.
** It is required to call Init before using the pool. No heap memory is
** allocated outside the Init function.
** After that use Alloc / Free / Resolve / Data to access the pool functionality.
** The memory is deallocated in the destructor.
**
/* -- implementation ---------------------------------------------------------
**
** Internally the pool manages n linear buffers, each of size sizeof(Type_n).
** A roster array is used as an indirection to access pool memoty, so alloc is 
** simply the operation of increasing roster partition, and freeing consists of
** decreasing it and swapping two roster indexes.
** It should be noted that freeing and allocating makes the pool non-linear for
** access operations, so if the cost of non-linear memory access is too high on
** the target platform (not the case with x86), it would be benifical to re-sort
** the roster at some points.
**
** -------------------------------------------------------------------------*/

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

	void RosterPool::Init(const uint16_t max_item_count, std::initializer_list<size_t> item_size)
	{
		buffer_count = static_cast<uint8_t>(item_size.size());

		this->item_size = new size_t[buffer_count];
		buffer = new uint8_t*[buffer_count];
		for (uint8_t i = 0; i < buffer_count; ++i)
		{
			this->item_size[i] = item_size.begin()[i];
			buffer[i] = reinterpret_cast<uint8_t*>(malloc(max_item_count * this->item_size[i]));
			assert(buffer[i] != nullptr);
		}

		this->max_item_count = max_item_count;
		item_partition = 0;
		
		indexes = new uint16_t[max_item_count];
		rev_indexes = new uint16_t[max_item_count];
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

		delete[] buffer;
		delete[] item_size;

		delete[] indexes;
		delete[] rev_indexes;
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

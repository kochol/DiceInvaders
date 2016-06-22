/* ---------------------------------------------------------------------------
** 
** roster_pool.h
** Multi-buffer memory pool, based on Insomniac's implementation, allowing
** very fast free/alloc operations
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once
#include <cstdint>
#include <initializer_list>

namespace Engine
{
	class RosterPool
	{
	private:
		uint8_t **buffer;
		
		uint8_t buffer_count;
		size_t *item_size;

		uint16_t max_item_count;
		uint16_t item_partition;

		// TODO: maintain two seperate sorted index segments
		uint16_t* indexes;
		uint16_t* rev_indexes;

	public:
		RosterPool();

		// - allocates the internal buffers
		// - _max_item_count is the pool item count
		// - _item_size defines two things:
		//    - its count indicates the number of data arrays
		//    - each item indicates the data type size of the coreresponding array
		void Init(const uint16_t _max_item_count, std::initializer_list<size_t> _item_size);
		~RosterPool();

		// - alloc / free from the pool.
		// - the handle could be used directly as an index into the pool memory buffers
		uint16_t Alloc();
		void Free(const uint16_t handle);


		// - data access helpers.
		// - the handle returned in the alloc operation could be used directly.
		// - if the intended operation is to access the i-th element of the pool,
		//   index buffer should be used as an indirection.
		inline void* Resolve(const uint16_t handle, const uint8_t _buffer_id = 0) const { return buffer[_buffer_id] + (handle * item_size[_buffer_id]); }
		inline void* Data(const uint8_t _buffer_id = 0) const { return buffer[_buffer_id]; }

		template<typename T>
		inline T* Resolve(const uint16_t handle, const uint8_t _buffer_id = 0) { return reinterpret_cast<T*>(Resolve(handle, _buffer_id)); }
		template<typename T>
		inline T* Data(const uint8_t _buffer_id = 0) const { return reinterpret_cast<T*>(Data(_buffer_id)); }

		const inline uint16_t* Indexes() const { return indexes; }

		// - the number of allocated objects
		inline uint16_t Size() const { return item_partition; }
	};
}

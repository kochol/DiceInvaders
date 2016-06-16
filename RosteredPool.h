#pragma once
#include <cstdint>

namespace Engine
{
	class RosteredPool
	{
	private:
		uint8_t* buffer;
		size_t item_size;
		uint16_t max_item_count;
		uint16_t item_partition;

		uint16_t* indexes;

	public:

		RosteredPool();
		void Init(const uint16_t _max_item_count, const size_t _item_size);
		~RosteredPool();

		uint16_t Alloc();
		void Free(uint16_t handle);
		void* Resolve(uint16_t handle) const;
	};
}

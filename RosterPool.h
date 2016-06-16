#pragma once
#include <cstdint>

namespace Engine
{
	class RosterPool
	{
	private:
		uint8_t* buffer;
		size_t item_size;
		uint16_t max_item_count;
		uint16_t item_partition;

		// TODO: maintain two seperate sorted index list
		uint16_t* indexes;

	public:

		RosterPool();
		void Init(const uint16_t _max_item_count, const size_t _item_size);
		~RosterPool();

		uint16_t Alloc();
		void Free(const uint16_t handle);

		inline void* Resolve(const uint16_t handle) const { return buffer + (handle * item_size); }
		inline void* Data() const { return buffer; }
		const inline uint16_t* Indexes() const { return indexes; }
		inline uint16_t Size() const { return item_partition; }
	};

	template<class T>
	class RosterPoolTyped
	{
	private:
		RosterPool _pool;
	public:
		inline void Init(const uint16_t _max_item_count) { _pool.Init(_max_item_count, sizeof(T)); }
		
		inline uint16_t Alloc() { return _pool.Alloc(); }
		inline void Free(uint16_t handle) { _pool.Free(handle); }

		inline T* Resolve(uint16_t handle) const { return reinterpret_cast<T*>(_pool.Resolve(handle)); }
		inline T* Data() const { return reinterpret_cast<T*>(_pool.Data()); }
		const inline uint16_t* Indexes() const { return _pool.Indexes(); }
		inline uint16_t Size() const { return _pool.Size(); }
	};
}

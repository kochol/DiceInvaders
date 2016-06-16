#pragma once
#include <cstdint>

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

	public:

		RosterPool();
		void Init(const uint16_t _max_item_count, const size_t _item_size);
		void Init(const uint8_t _buffer_count, const uint16_t _max_item_count, const size_t *const _item_size);
		~RosterPool();

		uint16_t Alloc();
		void Free(const uint16_t handle);

		inline void* Resolve(const uint16_t handle) const { return Resolve(0, handle); }
		inline void* Data() const { return Data(0); }
		inline void* Resolve(const uint8_t _buffer_id, const uint16_t handle) const { return buffer[_buffer_id] + (handle * item_size[_buffer_id]); }
		inline void* Data(const uint8_t _buffer_id) const { return buffer[_buffer_id]; }

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

	// TODO: maybe variadic templates could help
	template<class T1, class T2>
	class RosterPoolPairTyped
	{
	private:
		RosterPool _pool;
	public:
		inline void Init(const uint16_t _max_item_count)
		{
			const size_t item_size[2] = { sizeof(T1), sizeof(T1) };
			_pool.Init(2, _max_item_count, item_size);
		}

		inline uint16_t Alloc() { return _pool.Alloc(); }
		inline void Free(uint16_t handle) { _pool.Free(handle); }

		inline T1* ResolveFirst(uint16_t handle) const { return reinterpret_cast<T1*>(_pool.Resolve(0, handle)); }
		inline T1* DataFirst() const { return reinterpret_cast<T1*>(_pool.Data(0)); }
		inline T2* ResolveSecond(uint16_t handle) const { return reinterpret_cast<T2*>(_pool.Resolve(1, handle)); }
		inline T2* DataSecond() const { return reinterpret_cast<T2*>(_pool.Data(1)); }

		const inline uint16_t* Indexes() const { return _pool.Indexes(); }
		inline uint16_t Size() const { return _pool.Size(); }
	};
}

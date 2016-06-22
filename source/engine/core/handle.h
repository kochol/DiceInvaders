/* ---------------------------------------------------------------------------
**
** handle.h
** Engine handle types and utilities
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace Engine
{
	template<typename T>
	struct alignas(4) TypedHandle
	{
		union
		{
			uint32_t _handle;
			struct
			{
				T header;
				uint16_t index;
			};
			
		};
	};
	
	template<typename T>
	struct HandleHasher
	{
		size_t operator()(const T& h) const
		{
			return static_cast<size_t>(h._handle);
		}
	};

	template<typename T>
	struct HandleComparer
	{
		bool operator()(const T& a, const T& b) const
		{
			return a._handle == b._handle;
		}
	};

	template<typename T1, typename T2>
	using HandleHashMap = std::unordered_map<T1, T2, HandleHasher<T1>, HandleComparer<T1>>;

	template<typename T>
	using HandleHashSet = std::unordered_set<T, HandleHasher<T>, HandleComparer<T>>;

	struct alignas(2) ResourceHandleHeader
	{
		uint8_t type;
		uint8_t padding;
	};
	using ResourceHandle = TypedHandle<ResourceHandleHeader>;

	struct alignas(2) EntityHandleHeader
	{
		uint8_t salt;
		uint8_t layer;
	};
	using EntityHandle = TypedHandle<EntityHandleHeader>;

	struct alignas(2) ComponentHandleHeader
	{
		uint8_t type;
		uint8_t layer;
	};
	using ComponentHandle = TypedHandle<ComponentHandleHeader>;
}
#pragma once
#include <cstdint>
#include <unordered_map>
#include "Enums.h"

namespace Engine
{
	template<class T>
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

		operator uint32_t() const { return _handle; }
		
		TypedHandle& operator=(const uint32_t _handle)
		{
			this->_handle = _handle;
			return *this;
		}

		TypedHandle& operator=(const TypedHandle& handle)
		{
			this->_handle = handle._handle;
			return *this;
		}
	};

	template<typename T1, typename T2>
	using HandleHashMap = std::unordered_map<T1, T2, std::hash<uint32_t>, std::equal_to<uint32_t>>;

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
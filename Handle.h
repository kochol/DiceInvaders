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

		/*size_t operator()(const TypedHandle& handle) const
		{
			return handle._handle;
		}

		bool operator()(const TypedHandle& a, const TypedHandle& b) const
		{
			return a._handle == b._handle;
		}*/
	};

	template<typename T1, typename T2>
	using HandleHashMap = std::unordered_map<T1, T2, std::hash<uint32_t>, std::equal_to<uint32_t>>;

	struct alignas(2) ResourceHandleHeader
	{
		ResourceType type;
		uint8_t padding;
	};
	using ResourceHandle = TypedHandle<ResourceHandleHeader>;

	struct alignas(2) EntityHandleHeader
	{
		uint8_t salt;
		LayerId layer;
	};
	using EntityHandle = TypedHandle<EntityHandleHeader>;

	struct alignas(2) ComponentHandleHeader
	{
		ComponentType type;
		LayerId layer;
	};
	using ComponentHandle = TypedHandle<ComponentHandleHeader>;
}
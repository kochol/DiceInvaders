#pragma once
#include <cstdint>

namespace Engine
{
	enum struct ResourceType : uint8_t
	{
		NONE = 0,
		SPRITE = 1
	};

	enum struct LayerId : uint8_t
	{
		NONE = 0,
		PLAYER = 1,
		ALIEN = 2,
		ROCKET = 3,
		BOMB = 4,

		MAX = BOMB + 1
	};

	enum struct ComponentType : uint8_t
	{
		NONE = 0,
		MODEL = 1,
		PLAYER = 2,
		ALIEN = 3,
		ROCKET = 4,
		BOMB = 5
	};

	enum struct CallbackStage
	{
		NONE = 0,
		INIT = 1,
		PRE_UPDATE = 2,
		REG_UPDATE = 3,
		POST_UPDATE = 4,
		SHUTDOWN = 5
	};
}
#pragma once

#include "Engine.h"

namespace Game
{
	struct Player : Engine::BaseComponent
	{
		float lastFired;
		uint8_t health;
	};
}
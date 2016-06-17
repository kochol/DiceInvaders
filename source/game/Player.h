#pragma once

#include "../engine/Engine.h"

namespace Game
{
	struct Player : Engine::BaseComponent
	{
		float lastFired;
		uint8_t health;
	};

	void InitPlayerManager(Engine::ComponentManager *const manager);
	void UpdatePlayerFromInput(Engine::ComponentManager* const manager);
	void ShutdownPlayerManager(Engine::ComponentManager* const manager);
}
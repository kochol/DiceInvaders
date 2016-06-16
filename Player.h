#pragma once

#include "Engine.h"

namespace Game
{
	struct Player : Engine::BaseComponent
	{
		float lastFired;
		uint8_t health;
	};

	void InitPlayerManager(Engine::ComponentManager *const manager);

	Engine::EntityHandle CreatePlayer();

	void UpdatePlayerFromInput(const Engine::EntityHandle player);
}
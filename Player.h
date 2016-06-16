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

	Engine::EntityHandle CreatePlayer(Engine::Context *const context);

	void UpdatePlayerFromInput(const float dt, const Engine::EntityHandle playerEntity, Engine::World *const world, const Engine::KeyStatus& keys);
}
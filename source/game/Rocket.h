#pragma once

#include "../engine/Engine.h"

namespace Game
{
	struct Rocket : Engine::BaseComponent
	{
	};

	void InitRocketManager(Engine::ComponentManager *const manager);

	void HandleRocketCollisions(Engine::ComponentManager* const manager);
	void UpdateRockets(Engine::ComponentManager* const manager);

	void ShutdownRocketManager(Engine::ComponentManager* const manager);
}
#pragma once

#include "../engine/Engine.h"

namespace Game
{
	struct Alien : Engine::BaseComponent
	{
		float lastDroped;
	};

	void InitAlienManager(Engine::ComponentManager *const manager);

	void HandleAlienCollisions(Engine::ComponentManager* const manager);
	void UpdateAliens(Engine::ComponentManager* const manager);
	void SpawnAliens(Engine::ComponentManager* const manager);

	void ShutdownAliens(Engine::ComponentManager* const manager);
}

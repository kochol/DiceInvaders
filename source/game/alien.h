/* ---------------------------------------------------------------------------
**
** alien.h
** The Alien component
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once

#include "../engine/engine.h"

namespace Game
{
	struct Alien
	{
		float lastDroped;
	};

	void InitAlienManager(Engine::ComponentManager::LayerData *const data);

	void HandleAlienCollisions(Engine::ComponentManager::LayerData *const data);
	void UpdateAliens(Engine::ComponentManager::LayerData *const data);
	void SpawnAliens(Engine::ComponentManager::LayerData *const data);

	void ShutdownAlienManager(Engine::ComponentManager::LayerData *const data);
}

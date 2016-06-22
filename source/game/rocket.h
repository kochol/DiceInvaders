/* ---------------------------------------------------------------------------
**
** rocket.h
** The Rocket component
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once

#include "../engine/engine.h"

namespace Game
{
	void InitRocketManager(Engine::ComponentManager::LayerData *const data);

	void HandleRocketCollisions(Engine::ComponentManager::LayerData *const data);
	void UpdateRockets(Engine::ComponentManager::LayerData *const data);

	void ShutdownRocketManager(Engine::ComponentManager::LayerData *const data);
}
/* ---------------------------------------------------------------------------
**
** bomb.h
** The Bomb component
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once

#include "../engine/engine.h"

namespace Game
{
	void InitBombManager(Engine::ComponentManager::LayerData *const data);

	void HandleBombCollisions(Engine::ComponentManager::LayerData *const data);
	void UpdateBombs(Engine::ComponentManager::LayerData *const data);

	void ShutdownBombManager(Engine::ComponentManager::LayerData *const data);
}

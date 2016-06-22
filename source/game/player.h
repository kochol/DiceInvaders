/* ---------------------------------------------------------------------------
**
** player.h
** The Player component
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once

#include "../engine/engine.h"

namespace Game
{
	struct Player
	{
		float lastFired;
		uint8_t health;
	};

	void InitPlayerManager(Engine::ComponentManager::LayerData *const data);
	void UpdatePlayerFromInput(Engine::ComponentManager::LayerData *const data);
	void ShutdownPlayerManager(Engine::ComponentManager::LayerData *const data);
}
#pragma once
#include "../engine/Engine.h"

namespace Game
{
	struct Bomb : Engine::BaseComponent
	{
	};

	void InitBombManager(Engine::ComponentManager *const manager);

	void HandleBombCollisions(Engine::ComponentManager* const manager);
	void UpdateBombs(Engine::ComponentManager* const manager);

	void ShutdownBombManager(Engine::ComponentManager* const manager);
}

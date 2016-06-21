#pragma once
#include "../engine/Engine.h"

namespace Game
{
	enum struct GameState
	{
		NONE = 0,
		PRE_GAME = 1,
		IN_GAME = 2,
		POST_GAME = 3
	};

	struct GameSession
	{
		Engine::ComponentHandle player;
		uint32_t score = 0;
		GameState state;
	};

	extern GameSession g_currentSession;

	void DamagePlayer();
	void ScorePlayer();

	Engine::EntityHandle SpawnPlayer();
	void DestroyPlayer();
	Engine::EntityHandle SpawnRocket(float x, float y);
	Engine::EntityHandle SpawnBomb(float x, float y);
	void DestroyAliens();

	void StartGame();
	void GameOver();

}

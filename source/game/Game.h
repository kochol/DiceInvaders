#pragma once
#include "../engine/Engine.h"

namespace Game
{
	enum struct GameState
	{
		NONE = 0,
		IN_GAME = 1,
		POST_GAME = 2
	};

	struct GameSession
	{
		Engine::ComponentHandle player;
		uint32_t score = 0;
		GameState state;
	};

	extern GameSession g_currentSession;

	Engine::EntityHandle SpawnPlayer();
	void DamagePlayer();
	void ScorePlayer();

	Engine::EntityHandle SpawnRocket(float x, float y);

	Engine::EntityHandle SpawnBomb(float x, float y);

	void StartGame();
	void GameOver();

}

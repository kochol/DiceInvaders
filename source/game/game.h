/* ---------------------------------------------------------------------------
**
** game.h
** Global game session logic
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once
#include "../engine/engine.h"

namespace Game
{
	enum ComponentDataSegment
	{
		COMPONENT_DATA_BASE = 0,
		COMPONENT_DATA_CUSTOM = 1
	};

	enum GameState
	{
		GAME_STATE_NONE = 0,
		GAME_STATE_PRE_GAME = 1,
		GAME_STATE_IN_GAME = 2,
		GAME_STATE_POST_GAME = 3
	};

	struct GameSession
	{
		Engine::ComponentHandle player;
		uint32_t hi_score = 0;
		uint32_t score = 0;
		GameState state = GAME_STATE_PRE_GAME;
	};

	extern GameSession g_currentSession;

	void Start();
	void End();
	void Update();
	inline GameState State() { return g_currentSession.state; }
	inline uint32_t HiScore() { return g_currentSession.hi_score; }
	inline uint32_t Score() { return g_currentSession.score; }
	inline Engine::ComponentHandle CurrentPlayer() { return g_currentSession.player; }

	void DamagePlayer();
	void ScorePlayer();

	Engine::EntityHandle SpawnPlayer();
	void DestroyPlayer();
	Engine::EntityHandle SpawnRocket(float x, float y);
	Engine::EntityHandle SpawnBomb(float x, float y);
	void DestroyAliens();
	void DestroyRockets();
	void DestroyBombs();
}

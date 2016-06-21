#include "Game.h"

namespace Game
{
	GameSession g_currentSession;

	void GameOver()
	{
		DestroyPlayer();
		DestroyAliens();

		g_currentSession.state = GameState::POST_GAME;
	}

	void StartGame()
	{
		SpawnPlayer();

		g_currentSession.state = GameState::IN_GAME;
	}

	void GameUpdate()
	{
		if (g_currentSession.state == GameState::IN_GAME)
		{
			Engine::Update();
		}
		else if (g_currentSession.state == GameState::PRE_GAME)
		{
			if (Engine::GetFrameData().keys.fire)
			{
				StartGame();
			}
		}
		else if (g_currentSession.state == GameState::POST_GAME)
		{
			if (Engine::GetFrameData().keys.fire)
			{
				g_currentSession.state = GameState::PRE_GAME;
			}
		}
	}
}
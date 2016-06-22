#include "Game.h"
#include "Player.h"

namespace Game
{
	GameSession g_currentSession;

	static void DrawInGameUi();
	static void DrawPreGameUi();
	static void DrawPostGameUi();

	void Start()
	{
		SpawnPlayer();
		g_currentSession.score = 0;
		g_currentSession.state = GAME_STATE_IN_GAME;
	}

	void End()
	{
		DestroyPlayer();
		DestroyAliens();
		DestroyBombs();
		DestroyRockets();

		g_currentSession.hi_score = max(g_currentSession.hi_score, g_currentSession.score);
		
		g_currentSession.state = GAME_STATE_POST_GAME;
	}

	void Update()
	{
		if (g_currentSession.state == GAME_STATE_IN_GAME)
		{
			DrawInGameUi();
		}
		else if (g_currentSession.state == GAME_STATE_PRE_GAME)
		{
			DrawPreGameUi();
			if (Engine::GetFrameData().keys.fire)
			{
				Start();
			}
		}
		else if (g_currentSession.state == GAME_STATE_POST_GAME)
		{
			DrawPostGameUi();
			if (Engine::GetFrameData().keys.fire)
			{
				g_currentSession.state = GAME_STATE_PRE_GAME;
			}
		}
	}

	void ScorePlayer()
	{
		g_currentSession.score += 10;
	}

	static void DrawInGameUi()
	{
		const int char_width = 7;

		const Player *const player = Engine::ResolveComponentSegment<Player>(CurrentPlayer(), COMPONENT_DATA_CUSTOM);

		char buffer[100];

		sprintf_s(buffer, "SCORE<1>");
		Engine::DrawText((Engine::ScreenWidth() - 170 - char_width * strlen(buffer)) / 2, 0, buffer);
		sprintf_s(buffer, "%04d", Score());
		Engine::DrawText((Engine::ScreenWidth() - 170 - char_width * strlen(buffer)) / 2, 20, buffer);

		sprintf_s(buffer, "HI-SCORE");
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, 0, buffer);
		sprintf_s(buffer, "%04d", HiScore());
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, 20, buffer);

		sprintf_s(buffer, "HEALTH<1>");
		Engine::DrawText((Engine::ScreenWidth() + 170 - char_width * strlen(buffer)) / 2, 0, buffer);
		sprintf_s(buffer, "%04d", player->health);
		Engine::DrawText((Engine::ScreenWidth() + 170 - char_width * strlen(buffer)) / 2, 20, buffer);
	}

	static void DrawPreGameUi()
	{
		const int char_width = 7;
		char buffer[100];

		sprintf_s(buffer, "SCORE<1>");
		Engine::DrawText((Engine::ScreenWidth() - 170 - char_width * strlen(buffer)) / 2, 0, buffer);
		sprintf_s(buffer, "%04d", Score());
		Engine::DrawText((Engine::ScreenWidth() - 170 - char_width * strlen(buffer)) / 2, 20, buffer);

		sprintf_s(buffer, "HI-SCORE");
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, 0, buffer);
		sprintf_s(buffer, "%04d", HiScore());
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, 20, buffer);

		sprintf_s(buffer, "HEALTH<1>");
		Engine::DrawText((Engine::ScreenWidth() + 170 - char_width * strlen(buffer)) / 2, 0, buffer);

		sprintf_s(buffer, "THE DICE INVADERS");
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, Engine::ScreenHeight() / 2 - 130, buffer);
		sprintf_s(buffer, "PRESENTS");
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, Engine::ScreenHeight() / 2 - 100, buffer);

		sprintf_s(buffer, "PUSH");
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, Engine::ScreenHeight() / 2 - 30, buffer);
		sprintf_s(buffer, "FIRE BUTTON TO START");
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, Engine::ScreenHeight() / 2, buffer);

	}

	static void DrawPostGameUi()
	{
		const int char_width = 7;

		char buffer[100];

		sprintf_s(buffer, "SCORE<1>");
		Engine::DrawText((Engine::ScreenWidth() - 170 - char_width * strlen(buffer)) / 2, 0, buffer);
		sprintf_s(buffer, "%04d", Score());
		Engine::DrawText((Engine::ScreenWidth() - 170 - char_width * strlen(buffer)) / 2, 20, buffer);

		sprintf_s(buffer, "HI-SCORE");
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, 0, buffer);
		sprintf_s(buffer, "%04d", HiScore());
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, 20, buffer);

		sprintf_s(buffer, "HEALTH<1>");
		Engine::DrawText((Engine::ScreenWidth() + 170 - char_width * strlen(buffer)) / 2, 0, buffer);

		sprintf_s(buffer, "GAME OVER!");
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, Engine::ScreenHeight() / 2 - 50, buffer);

		sprintf_s(buffer, "PUSH");
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, Engine::ScreenHeight() / 2, buffer);
		sprintf_s(buffer, "FIRE BUTTON TO START");
		Engine::DrawText((Engine::ScreenWidth() - char_width * strlen(buffer)) / 2, Engine::ScreenHeight() / 2 + 30, buffer);
	}
}

#include "Scenes.h"

GameEngine* game_engine = nullptr;

// Pointers to scenes
MainMenu* main_menu = nullptr;
Chessboard* chessboard = nullptr;

int main(int argc, char* args[])
{
	// Timer
	int const frameDelay = 1000 / FPS;    // (Time delay for one frame) = (1000)/(frames per second)
	Uint32 frameStart;
	Uint32 elapsedTime;
	Uint32 previousTime = SDL_GetTicks();
	Uint32 lag = 0;
	int deltaTime;
	bool allowEvent = true;

	// Game Initialising
	game_engine = new GameEngine();
	game_engine->Init("Chess Game Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_FULLSCREEN);

	// Game Loop
	GameEngine::stage = 0;
	GameEngine::initialize_stage = true;

	while (game_engine->Running())
	{
		frameStart = SDL_GetTicks();
		elapsedTime = frameStart - previousTime;
		previousTime = frameStart;
		lag += elapsedTime;

		game_engine->EventHandler(&allowEvent);

		// Throttle event processing based on a cooldown period
		if (allowEvent && lag >= frameDelay) {
			allowEvent = false;
			lag = 0;
		}
		else if (!allowEvent) {
			if (elapsedTime >= frameDelay) {
				allowEvent = true;
			}
		}

		// Stages
		switch (GameEngine::stage)
		{
			// Main menu
			case 0:
				if (GameEngine::initialize_stage)
				{
					std::cout << "Main menu initialized!" << std::endl;

					if (main_menu != nullptr)
					{
						delete main_menu;
						main_menu = nullptr;
					}

					main_menu = new MainMenu();
					GameEngine::mouse_left = false;
					GameEngine::initialize_stage = false;
					GameEngine::human_player = BLACK_FIGURES;
				}

				Main_menu_scene(main_menu);

				if (GameEngine::stage != 0)
				{
					delete main_menu;
					main_menu = nullptr;
					GameEngine::initialize_stage = true;
				}
				break;

			// Chessboard
			case 1:
				if (GameEngine::initialize_stage)
				{
					std::cout << "Chessboard initialized!" << std::endl;

					if (chessboard != nullptr)
					{
						delete chessboard;
						chessboard = nullptr;
					}

					if (GameEngine::human_player == WHITE_FIGURES)
					{
						GameEngine::human_player = BLACK_FIGURES;
					}
					else
					{
						GameEngine::human_player = WHITE_FIGURES;
					}

					chessboard = new Chessboard(64);
					GameEngine::mouse_left = false;
					GameEngine::initialize_stage = false;
				}

				Chessboard_scene(chessboard);

				if (GameEngine::stage != 1)
				{
					delete chessboard;
					chessboard = nullptr;
					GameEngine::initialize_stage = true;
				}
				break;

			// Exit
			case 2:
				GameEngine::isRunning = false;
				break;

			default:
				GameEngine::isRunning = false;
				break;
		}

		deltaTime = SDL_GetTicks() - frameStart;

		// Frame Delay
		if (frameDelay > deltaTime)
		{
			SDL_Delay(frameDelay - deltaTime);
		}
	}

	game_engine->Clean();

	return 0;
}

#pragma once

#include "GameEngine.h"

// Scenes
#include "MainMenu.h"
#include "Chessboard.h"

void Main_menu_scene(MainMenu* main_menu)
{
	switch (main_menu->scene)
	{
		// Main menu
		case 0:
			if (main_menu->initialize_scene)
			{
				SDL_SetRenderDrawColor(GameEngine::renderer, 0, 0, 0, 255);
				main_menu->initialize_scene = false;
				GameEngine::mouse_left = false;
			}
			main_menu->UpdateMainMenu();
			main_menu->RenderMainMenu();

			if (main_menu->scene != 0)
			{
				main_menu->initialize_scene = true;
			}
			break;


		// Start Game
		case 1:
			GameEngine::stage = 1;
			GameEngine::initialize_stage = true;
			break;

		// Options
		case 2:
			if (main_menu->initialize_scene)
			{
				SDL_SetRenderDrawColor(GameEngine::renderer, 0, 0, 0, 255);
				main_menu->initialize_scene = false;
				GameEngine::mouse_left = false;
			}

			main_menu->UpdateOptions();
			main_menu->RenderOptions();

			if (main_menu->scene != 2)
			{
				main_menu->initialize_scene = true;
			}
			break;

		// Exit
		case 3:
			GameEngine::isRunning = false;
			break;

		default:
			GameEngine::isRunning = false;
			break;
	}
}

void Chessboard_scene(Chessboard* chessboard)
{
	chessboard->BoardUpdate();
	chessboard->UpdateFigures();
	chessboard->RenderFigures();
	chessboard->SwitchTurns();
	chessboard->EndGame();
}

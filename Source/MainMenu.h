#pragma once

#include "GameEngine.h"

class MainMenu
{
	private:
		int selected;
		Text* marked;
		bool clicked;
		SDL_Color marked_color;

	public:
		MainMenu();
		~MainMenu();

		// Main menu
		Text menu_selection[5];

		// Options
		Text options_selection[3];

		// Scenes handling
		int scene;
		bool initialize_scene;

		// Create scene
		void CreateMainMenu();
		void CreateOptions();

		// Update scene
		void UpdateMainMenu();
		void UpdateOptions();

		// Render scene
		void RenderMainMenu();
		void RenderOptions();
};

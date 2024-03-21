#include "MainMenu.h"

MainMenu::MainMenu()
{
	this->selected = 0;
	this->marked = nullptr;
	this->scene = 0;
	this->initialize_scene = true;

	CreateMainMenu();
	CreateOptions();
}

MainMenu::~MainMenu() {}

void MainMenu::CreateMainMenu()
{
	for (int i = 0; i < 5; i++)
	{
		menu_selection[i].color = { 255, 255, 255 };
	}

	menu_selection[0].text = "Chess Game Project";
	menu_selection[0].rect = GameEngine::CreateRectangle(SCREEN_WIDTH/2 - 200, 60, 400, 100);
	menu_selection[0].unselected = TextureMenager::LoadFont(menu_selection[0].text, menu_selection[0].color);
	menu_selection[0].selected = TextureMenager::LoadFont(menu_selection[0].text, { 255, 0, 0 });

	menu_selection[1].text = "Start Game";
	menu_selection[1].rect = GameEngine::CreateRectangle(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 70, 200, 50);
	menu_selection[1].unselected = TextureMenager::LoadFont(menu_selection[1].text, menu_selection[1].color);
	menu_selection[1].selected = TextureMenager::LoadFont(menu_selection[1].text, { 255, 0, 0 });

	menu_selection[2].text = "Options";
	menu_selection[2].rect = GameEngine::CreateRectangle(SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 - 20, 120, 50);
	menu_selection[2].unselected = TextureMenager::LoadFont(menu_selection[2].text, menu_selection[2].color);
	menu_selection[2].selected = TextureMenager::LoadFont(menu_selection[2].text, { 255, 0, 0 });

	menu_selection[3].text = "Exit Game";
	menu_selection[3].rect = GameEngine::CreateRectangle(SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 30, 160, 50);
	menu_selection[3].unselected = TextureMenager::LoadFont(menu_selection[3].text, menu_selection[3].color);
	menu_selection[3].selected = TextureMenager::LoadFont(menu_selection[3].text, { 255, 0, 0 });

	menu_selection[4].text = "Author: Maciej Kaptur";
	menu_selection[4].rect = GameEngine::CreateRectangle(SCREEN_WIDTH - 190, SCREEN_HEIGHT - 45, 180, 40);
	menu_selection[4].unselected = TextureMenager::LoadFont(menu_selection[4].text, menu_selection[4].color);
	menu_selection[4].selected = TextureMenager::LoadFont(menu_selection[4].text, { 255, 0, 0 });
}

void MainMenu::CreateOptions()
{
	for (int i = 0; i < 1; i++)
	{
		options_selection[i].color = { 255, 255, 255 };
	}

	options_selection[0].text = "Back";
	options_selection[0].rect = GameEngine::CreateRectangle(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 45, 90, 40);
	options_selection[0].unselected = TextureMenager::LoadFont(options_selection[0].text, options_selection[0].color);
	options_selection[0].selected = TextureMenager::LoadFont(options_selection[0].text, { 255, 0, 0 });
}

void MainMenu::UpdateMainMenu()
{
	// Check for selection from menu
	for (int select = 1; select < 4; select++)
	{
		if (marked == nullptr)
		{
			if (GameEngine::CollisionDetector(GameEngine::mouse_x, GameEngine::mouse_y, &menu_selection[select].rect))
			{
				marked = &menu_selection[select];
				selected = select;
			}
		}
		else
		{
			if (!GameEngine::CollisionDetector(GameEngine::mouse_x, GameEngine::mouse_y, &marked->rect))
			{
				marked = nullptr;
				selected = 0;
			}
			else
			{
				if (GameEngine::mouse_left)
				{
					marked = nullptr;
					scene = selected;
					break;
				}
			}
		}
	}
}

void MainMenu::UpdateOptions()
{
	// Check for selection from options
	for (int select = 0; select < 1; select++)
	{
		if (marked == nullptr)
		{
			if (GameEngine::CollisionDetector(GameEngine::mouse_x, GameEngine::mouse_y, &options_selection[select].rect))
			{
				marked = &options_selection[select];
				selected = select;
			}
		}
		else
		{
			if (!GameEngine::CollisionDetector(GameEngine::mouse_x, GameEngine::mouse_y, &marked->rect))
			{
				marked = nullptr;
				selected = 0;
			}
			else
			{
				if (GameEngine::mouse_left)
				{
					marked = nullptr;
					scene = selected;
					break;
				}
			}
		}
	}
}

void MainMenu::RenderMainMenu()
{
	SDL_RenderClear(GameEngine::renderer);

	for (Text& selection : menu_selection)
	{
		TextureMenager::Draw(selection.unselected, selection.rect);
	}

	// Render selected
	if (marked != nullptr)
	{
		TextureMenager::Draw(marked->selected, marked->rect);
	}

	SDL_RenderPresent(GameEngine::renderer);
}

void MainMenu::RenderOptions()
{
	SDL_RenderClear(GameEngine::renderer);

	for (Text& selection : options_selection)
	{
		TextureMenager::Draw(selection.unselected, selection.rect);
	}

	// Render selected
	if (marked != nullptr)
	{
		TextureMenager::Draw(marked->selected, marked->rect);
	}

	SDL_RenderPresent(GameEngine::renderer);
}
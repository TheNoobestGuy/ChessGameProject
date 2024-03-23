#include "GameEngine.h"

// Renderer
SDL_Renderer* GameEngine::renderer = nullptr;

// Load scene
bool GameEngine::isRunning;
int GameEngine::stage;
bool GameEngine::initialize_stage;

// Game Handler
bool  GameEngine::reset_game;
bool  GameEngine::end_game;

// Keyboard tracer
bool GameEngine::key_down;

// Mouse tracer
bool GameEngine::mouse_left;
int GameEngine::mouse_x;
int GameEngine::mouse_y;

// Vital game functions
void GameEngine::Init(const char* title, int x, int y, int width, int height, bool fullscreen)
{
	int flags = 0;

	if (fullscreen)
		flags = SDL_WINDOW_FULLSCREEN;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		printf("Subsystem Initialised!\n");

		// Window Creating
		window = SDL_CreateWindow(title, x, y, width, height, flags);

		// Init SDL fonts
		TTF_Init();

		if (window)
		{
			printf("Windows had been created!\n");

			// Window Renderer
			renderer = SDL_CreateRenderer(window, -1, 0);
			if (renderer)
			{
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				printf("Renderer has been created!\n");

				GameEngine::isRunning = true;
			}
			else
			{
				printf("Render couldn't be created: %s\n", SDL_GetError());
				GameEngine::isRunning = false;
			}
		}
		else
		{
			printf("Window couldn't be created: %s\n", SDL_GetError());
			GameEngine::isRunning = false;
		}
	}
	else
	{
		printf("Subsystem failed to initialise: %s\n", SDL_GetError());
		GameEngine::isRunning = false;
	}
}

void GameEngine::EventHandler(bool allowEvent)
{
	SDL_Event event;

	SDL_GetMouseState(&mouse_x, &mouse_y);

	while (allowEvent && SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			GameEngine::isRunning = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				GameEngine::mouse_left = true;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				GameEngine::mouse_left = false;
			}
			break;

		case SDL_KEYDOWN:
			if (GameEngine::stage == 1 && !key_down)
			{
				if (event.key.keysym.sym == SDLK_r)
				{
					GameEngine::reset_game = true;
				}

				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					GameEngine::end_game = true;
				}
			}

			GameEngine::key_down = true;
			break;

		case SDL_KEYUP:
			GameEngine::key_down = false;
			break;

		default:
			break;
		}
	}
}

void GameEngine::Clean()
{
	TTF_Quit();

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	IMG_Quit();
	SDL_Quit();

	printf("Game Successfully Closed!\n");
}

// Collisions handler
SDL_Rect GameEngine::CreateRectangle(int pos_x, int pos_y, int size)
{
	SDL_Rect rect;

	rect.x = pos_x * size;
	rect.y = pos_y * size;
	rect.w = rect.h = size;

	return rect;
}

SDL_Rect GameEngine::CreateRectangle(int pos_x, int pos_y, int width, int height)
{
	SDL_Rect rect;

	rect.x = pos_x;
	rect.y = pos_y;
	rect.w = width;
	rect.h = height;

	return rect;
}

void GameEngine::RectangleHandler(SDL_Rect* srcRect, SDL_Rect* destRect, int src_size, int dest_size)
{
	// Source size and position of image
	srcRect->w = src_size;
	srcRect->h = src_size;
	srcRect->x = srcRect->y = 0;

	// Destinated size
	destRect->w = destRect->h = dest_size;
}

bool GameEngine::CollisionDetector(SDL_Rect* first_object, SDL_Rect* second_object)
{
	if (first_object->x + 32 >= second_object->x && first_object->x + 32 <= (second_object->x + second_object->w))
	{
		if (first_object->y + 32>= second_object->y && first_object->y + 32 <= (second_object->y + second_object->h)) {
			return true;
		}
	}
	return false;
}

bool GameEngine::CollisionDetector(int &x_coordinate, int &y_coordinate, SDL_Rect* object)
{
	if (x_coordinate >= object->x && x_coordinate <= (object->x + object->w))
	{
		if (y_coordinate >= object->y && y_coordinate <= (object->y + object->h)) {
			return true;
		}
	}
	return false;
}

#pragma once

// Basic Libraries
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <stack>

// SDL Libraries
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// Header files
#include "Constants.h"
#include "TextureMenager.h"

class GameEngine
{
	private:
		SDL_Window* window;

	public:
		static bool isRunning;
		static SDL_Renderer* renderer;

		// Vital game functions
		void Init(const char* title, int x, int y, int width, int height, bool fullscreen);
		void EventHandler(bool allowevent);
		void Clean();

		// Load scene
		static int stage;
		static bool initialize_stage;

		// Game scene
		static bool reset_game;
		static bool end_game;
		
		// Mouse handler
		static bool mouse_left;
		static int mouse_x;
		static int mouse_y;

		// Keyboard handler
		static bool key_down;

		// Collisions handler
		static SDL_Rect CreateRectangle(int pos_x, int pos_y, int size);
		static SDL_Rect CreateRectangle(int pos_x, int pos_y, int width, int height);
		static bool CollisionDetector(SDL_Rect* first_object, SDL_Rect* second_object);
		static bool CollisionDetector(int &x_coordinate, int &y_coordinate, SDL_Rect* object);
		static void RectangleHandler(SDL_Rect* srcRect, SDL_Rect* destRect, int src_size, int dest_size);

		// State of game engine
		bool Running() { return isRunning; }
};

// Structs
struct Field_ID
{
	int x;
	int y;

	bool available_move = true;
	int move_axis = 0;

	bool operator==(const Field_ID& field) const
	{
		return field.x == x && field.y == y;
	}

	bool operator!=(const Field_ID& field) const
	{
		return field.x != x || field.y != y;
	}
};

struct Texture
{
	SDL_Texture* texture;
	SDL_Rect srcRect = GameEngine::CreateRectangle(0, 0, 64);

	~Texture()
	{
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
};
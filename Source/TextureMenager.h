#pragma once

#include "GameEngine.h"

struct Text
{
	const char* text;
	SDL_Color color;
	SDL_Rect rect;

	int value = 0;
	SDL_Texture* unselected = nullptr;
	SDL_Texture* selected = nullptr;

	~Text()
	{
		text = nullptr;
		SDL_DestroyTexture(unselected);
		unselected = nullptr;
		SDL_DestroyTexture(selected);
		selected = nullptr;
	}
};

class TextureMenager
{
	public:
		static SDL_Texture* LoadTexture(const char* fileName);
		static SDL_Texture* LoadFont(char const* text, SDL_Color color);
		static void Draw(SDL_Texture* texture, SDL_Rect& srcRect, SDL_Rect& destRect);
		static void Draw(SDL_Texture* texture, SDL_Rect& rect);
};
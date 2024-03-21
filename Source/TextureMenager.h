#pragma once

#include "GameEngine.h"

struct Text
{
	const char* text;
	SDL_Color color;
	SDL_Rect rect;
	SDL_Texture* unselected = nullptr;
	SDL_Texture* selected = nullptr;
};

class TextureMenager
{
	public:
		static TTF_Font* font;
		static SDL_Texture* LoadTexture(const char* fileName);
		static SDL_Texture* LoadFont(char const* text, SDL_Color color);
		static void Draw(SDL_Texture* texture, SDL_Rect& srcRect, SDL_Rect& destRect);
		static void Draw(SDL_Texture* texture, SDL_Rect& rect);
};
#include "TextureMenager.h"

SDL_Texture* TextureMenager::LoadTexture(const char* fileName)
{
	SDL_Surface* tempSurface = IMG_Load(fileName);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(GameEngine::renderer, tempSurface);
	SDL_FreeSurface(tempSurface);

	return texture;
}

SDL_Texture* TextureMenager::LoadFont(char const* text, SDL_Color color)
{
	TTF_Font* font = TTF_OpenFont("Assets/Font/BebasNeue-Regular.ttf", 24);
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(GameEngine::renderer, surface);
	SDL_FreeSurface(surface);
	font = nullptr;

	return texture;
}

void TextureMenager::Draw(SDL_Texture* texture, SDL_Rect& srcRect, SDL_Rect& destRect)
{
	SDL_RenderCopy(GameEngine::renderer, texture, &srcRect, &destRect);
}

void TextureMenager::Draw(SDL_Texture* texture, SDL_Rect& rect)
{
	SDL_RenderCopy(GameEngine::renderer, texture, NULL, &rect);
}

#pragma once

#include "Figure.h"

class Knight : public Figure
{
	private:
		Field_ID movement[8] =
		{
			//				 X axis									     Y axis				
			{ -2, -1}, { -2, 1}, { 2, -1}, { 2, 1},    { -1, -2}, { 1, -2}, { 1, 2}, { -1, 2},
		};

		Texture knight_textures[2] =
		{
			{ TextureMenager::LoadTexture("Textures/Figures/white_knight.png") },
			{ TextureMenager::LoadTexture("Textures/Figures/black_knight.png") }
		};

	public:
		using Figure::Figure;

		void PossibleMoves()
		{
			for (Field_ID move : movement)
			{
				moves_list.push_back(move);
			}
		}

		void Render()
		{
			if (picked_up)
				TextureMenager::Draw(knight_textures[color].texture, knight_textures[color].srcRect, motion_rect);
			else
				TextureMenager::Draw(knight_textures[color].texture, knight_textures[color].srcRect, figure_rect);
		}
};


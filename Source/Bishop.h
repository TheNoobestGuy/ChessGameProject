#pragma once

#include "Figure.h"

class Bishop : public Figure
{
	private:
		Field_ID movement[4] =
		{
			//             Curve axis
			{ -1, -1}, { 1, -1}, { 1, 1}, { -1, 1}
		};

		Texture bishop_textures[2] =
		{
			{ TextureMenager::LoadTexture("Textures/Figures/white_bishop.png") },
			{ TextureMenager::LoadTexture("Textures/Figures/black_bishop.png") }
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
				TextureMenager::Draw(bishop_textures[color].texture, bishop_textures[color].srcRect, motion_rect);
			else
				TextureMenager::Draw(bishop_textures[color].texture, bishop_textures[color].srcRect, figure_rect);
		}
};


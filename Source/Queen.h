#pragma once

#include "Figure.h"

class Queen : public Figure
{
	private:
		Field_ID movement[8] =
		{
			//         Straight axis										  Curve axis
			{ 0, -1}, { 0, 1}, { -1, 0}, { 1, 0},				{ -1, -1}, { 1, -1}, { 1, 1}, { -1, 1}
		};

		Texture queen_textures[2] =
		{
			{ TextureMenager::LoadTexture("Textures/Figures/white_queen.png") },
			{ TextureMenager::LoadTexture("Textures/Figures/black_queen.png") }
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
				TextureMenager::Draw(queen_textures[color].texture, queen_textures[color].srcRect, motion_rect);
			else
				TextureMenager::Draw(queen_textures[color].texture, queen_textures[color].srcRect, figure_rect);
		}
};

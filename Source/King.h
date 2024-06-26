#pragma once

#include "Figure.h"

class King : public Figure
{
	private:
		Field_ID movement[8] =
		{
			//         Straight axis										  Curve axis
			{ 0, -1}, { 0, 1}, { -1, 0}, { 1, 0},				{ -1, -1}, { 1, -1}, { 1, 1}, { -1, 1}
		};

		Texture king_textures[2] =
		{
			{ TextureMenager::LoadTexture("Textures/Figures/white_king.png") },
			{ TextureMenager::LoadTexture("Textures/Figures/black_king.png") }
		};

	public:
		~King() {};
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
				TextureMenager::Draw(king_textures[color].texture, king_textures[color].srcRect, motion_rect);
			else
				TextureMenager::Draw(king_textures[color].texture, king_textures[color].srcRect, figure_rect);
		}
};


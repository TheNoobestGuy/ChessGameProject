#pragma once

#include "Figure.h"

class Rook : public Figure
{
	private:
		Field_ID movement[4] =
		{
			//         Straight axis
			 { 0, -1}, { 0, 1}, { -1, 0}, { 1, 0}
		};

		Texture rook_textures[2] =
		{
			{ TextureMenager::LoadTexture("Textures/Figures/white_rook.png") },
			{ TextureMenager::LoadTexture("Textures/Figures/black_rook.png") }
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
				TextureMenager::Draw(rook_textures[color].texture, rook_textures[color].srcRect, motion_rect);
			else
				TextureMenager::Draw(rook_textures[color].texture, rook_textures[color].srcRect, figure_rect);
		}
};


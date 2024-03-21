#pragma once

#include "Figure.h"

class Pawn : public Figure
{
	private:
		Field_ID movement[2][2] =
		{
			// White pawn
			{ { 0, -1}, { 0, -2} },

			{ { 0,  1}, { 0,  2} }
		};

		Field_ID attacks_list[3][2] =
		{
			// White pawn
			{ {-1, -1}, { 1, -1} },

			// Black pawn
			{ {-1,  1}, { 1,  1} },

			// En passant
			{ {-1,  0}, { 1,  0} }
		};

		Texture pawns_textures[2] =
		{
			{ TextureMenager::LoadTexture("Textures/Figures/white_pawn.png") },
			{ TextureMenager::LoadTexture("Textures/Figures/black_pawn.png") }
		};

	public:
		using Figure::Figure;

		void PossibleMoves()
		{
			for (int move = 0; move < 2; move++)
			{
				moves_list.push_back(movement[color][move]);
			}

			for (int attack = 0; attack < 2; attack++)
			{
				moves_list.push_back(attacks_list[color][attack]);
			}

			for (int en_passsant = 0; en_passsant < 2; en_passsant++)
			{
				moves_list.push_back(attacks_list[2][en_passsant]);
			}
		}

		void Render()
		{
			if (picked_up)
				TextureMenager::Draw(pawns_textures[color].texture, pawns_textures[color].srcRect, motion_rect);
			else
				TextureMenager::Draw(pawns_textures[color].texture, pawns_textures[color].srcRect, figure_rect);
		}
};


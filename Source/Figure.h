#pragma once

#include "GameEngine.h"

class Figure
{
	protected:
		// Properties
		std::string name;
		int ID;
		int color;
		int size;
		int player;
		int value;
		Field_ID occupied_field;

		// Movements
		bool entangled;
		bool en_passant;
		bool first_move;
		bool picked_up;

		// Collisions
		SDL_Rect figure_rect;
		SDL_Rect motion_rect;

	public:
		Figure(std::string name, int figure_ID, Field_ID field_ID, int color, int size, int value);
		virtual ~Figure();

		// Possible plays
		std::vector<Field_ID> available_moves;
		std::vector<Field_ID> entangled_moves;
		std::vector<Field_ID> way_to_opposite_king;
		std::vector<Field_ID> moves_list;

		// Figure features
		virtual void PossibleMoves() = 0;
		void PickUp(bool &figure_picked_up);

		// Draw function
		void ChangePosition(Field_ID field);
		void ChangePositionComputer(Field_ID field);
		virtual void Render() = 0;

		// Properties
		int GetID() { return ID; }
		std::string GetName() { return name; }
		Field_ID GetField() { return occupied_field; }

		void SetPlayer(int player) { this->player = player; }
		int GetPlayer() { return player; }
		int GetColor() { return color; }
		int GetValue() { return value; }

		bool IsItFirstMove() { return first_move; }
		void NotFirstMove() { first_move = false; }

		bool PickedUp() { return picked_up; }
		bool EnPassant() { return en_passant; }
		void EnPassantVulnerablity() { en_passant = true; }

		bool IsItEntangled() { return entangled; }
		void MakeEntangled() { entangled = true; };
		void Release() { entangled_moves.clear(); entangled = false; way_to_opposite_king.clear();}

		SDL_Rect* GetMotionRect() { return &motion_rect;  }
};

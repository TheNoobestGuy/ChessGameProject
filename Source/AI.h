#pragma once

#include "Figure.h"

struct Field
{
	// Field proporties
	Field_ID field_ID;
	int field_size;
	int color;

	// Figures
	Figure* figure = nullptr;
	bool en_passant = false;
	bool field_under_attack[2] = { false, false };

	// Rectangle
	SDL_Rect field_rect = GameEngine::CreateRectangle(field_ID.x, field_ID.y, field_size);

	~Field()
	{
		figure = nullptr;
	}
};

struct AI_Field
{
	Field_ID field_ID;
	Figure* figure;

	~AI_Field()
	{
		figure = nullptr;
	}
};

class AI
{
	private:
		std::vector<AI_Field> available_moves;
		AI_Field** converted_chessboard;
		AI_Field** boards[2];
		AI_Field best_move;

		// AI features
		void AttachAvailableMoves(std::vector<Figure*>& computer_figures);
		void ConvertBoard(Field* chessboard[][8]);

		// Minimax algorithm with alpha-beta pruning
		int EvaluateBoard(AI_Field** chessboard);
		int MiniMax(AI_Field** chessboard, int depth);

		AI_Field FindBestMove(AI_Field** chessboard, int depth);
		AI_Field** CheckMove(AI_Field** chessboard, AI_Field& move);

		void deleteChessboard(AI_Field** chessboard);

	public:
		AI(Field* chessboard[][8]);
		~AI();

		// AI update
		void UpdateAI(Field* chessboard[][8], std::vector<Figure*>& computer_figures);

		Figure* MoveFigure() { return best_move.figure; }
		Field_ID MoveToField() { return best_move.field_ID; }
};
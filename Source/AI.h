#pragma once

#include "Figure.h"

// Figures
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"

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

struct FigureMove
{
	int x = 0;
	int y = 0;
	Figure* attacked_figure = nullptr;

	~FigureMove()
	{
		attacked_figure = nullptr;
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
		// TEST
		std::vector<std::tuple<int, Field>> moves_values;

		Field best_move;

		// Evaluating moves algorithm
		int EvaluateBoard(Field* chessboard[][8], Field& move, bool computer_turn);
		void EvaluatingMovesAlgorithm(Field* chessboard[][8], Field move, std::vector<Figure*> player_figures, std::vector<Figure*> computer_figures, Figure* player_king, Figure* computer_king, Figure* figure_to_remove, bool& computer_turn, bool checkmate, int& value, int depth);

		void CheckMove(Field* chessboard[][8], Field* newChessboard[][8], Field& move);
		Field FindBestMove(Field* chessboard[][8], std::vector<Figure*> player_figures, std::vector<Figure*> computer_figures, Figure* player_king, Figure* computer_king, Figure* figure_to_remove, int depth);

		void deleteChessboard(Field* chessboard[][8]);

		// Board update features
		void RemoveFromBoard(Figure* figure_to_remove, std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures);
		void HasBecomeQueen(std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures);
		void AttachPositionsToBoard(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures);
		void CalculateFigureMoves(Field* chessboard[][8], std::vector<Figure*>& player_figures);
		void MarkFieldsUnderAttack(Field* chessboard[][8], std::vector<Figure*>& player_figures);
		void CheckForEntangling(Field* chessboard[][8], std::vector<Figure*>& player_figures, Figure* opposite_king);
		void ApplyEntangledMoves(std::vector<Figure*>& player_figures);
		void KingMechanic(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& opposite_player_figures, Figure* king, bool& checkmate);
		void RemoveUnavailableMoves(std::vector<Figure*>& player_figures);

	public:
		AI();
		~AI();

		// AI update
		void UpdateAI(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Figure* player_king, Figure* computer_king, Figure* figure_to_remove);
		void UpdateBoard(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Figure* player_king, Figure* computer_king, Figure* figure_to_remove, bool& checkmate);

		Figure* MoveFigure() { return best_move.figure; }
		Field_ID MoveToField() { return best_move.field_ID; }
};
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

class AI
{
	private:
		Field best_move;
	
		Text* marked;
		Text* chooseTopFigures[6];
		Text* chooseBottomFigures[6];
	
		// Evaluating moves algorithm
		Field FindBestMove(Field* chessboard[][8], int depth, std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Figure*& player_king, Figure*& computer_king, Figure*& figure_to_remove);
		int MiniMaxAlphaBetaPrunning(Field* chessboard[][8], std::vector<std::tuple<Field, int>>& moves, int depth, int& alpha, int& beta, int maximazing_player, std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Figure*& player_king, Figure*& computer_king, Figure*& figure_to_remove);
		int EvaluateBoard(Field* chessboard[][8], int maximazing_player);
	
		void MakeCopyOfFiguresForCalculatingMoves(std::vector<Figure*>& player_figures, std::vector<Figure*>& player_figures_update, Figure*& player_king_update);
		void DeleteFigures(std::vector<Figure*>& player_figures);

		void CreateNewChessboard(Field* previousChessboard[][8], Field* newChessboard[][8], Field& move);
		void DeleteCreatedChessboard(Field* chessboard[][8]);
	
		// Show choice when pawn has reached end of a chessboard
		void HasBecomeFigure(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures);
		void CreateChoiceWindow(int figures_color, int start_field, int field, int field_y, Text* options[6]);
		void ChoiceBetweenFigures(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, int& selected_figure, bool& running, int& choice);
		void EventHandlerForChoice(bool& running, int& choice);
		void UpdateChoice(Text* options[6], int& selected_figure, bool& running);
		void RenderChoice(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Text* options[6]);
	
		// Board update features
		void RemoveFromBoard(Figure* figure_to_remove, std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures);
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
	
		// Textures
		Texture fields_colors[2] =
		{
			{ TextureMenager::LoadTexture("Textures/Chessboard/whiteSqr.png") },
			{ TextureMenager::LoadTexture("Textures/Chessboard/blackSqr.png") }
		};
	
		Texture choiceBorders[2]
		{
			{ TextureMenager::LoadTexture("Textures/ChoiceOptions/BorderLeft.png") },
			{ TextureMenager::LoadTexture("Textures/ChoiceOptions/BorderRight.png") }
		};
	
		Texture choiceFiguresUnselected[2][4] =
		{
			{
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/WhiteFigureQueenUnselected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/WhiteFigureRookUnselected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/WhiteFigureKnightUnselected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/WhiteFigureBishopUnselected.png") }
			},
			{
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/BlackFigureQueenUnselected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/BlackFigureRookUnselected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/BlackFigureKnightUnselected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/BlackFigureBishopUnselected.png") }
			}
		};
	
		Texture choiceFiguresSelected[2][4] =
		{
			{
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/WhiteFigureQueenSelected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/WhiteFigureRookSelected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/WhiteFigureKnightSelected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/WhiteFigureBishopSelected.png") }
			},
			{
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/BlackFigureQueenSelected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/BlackFigureRookSelected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/BlackFigureKnightSelected.png") },
				{ TextureMenager::LoadTexture("Textures/ChoiceOptions/BlackFigureBishopSelected.png") }
			}
		};
};

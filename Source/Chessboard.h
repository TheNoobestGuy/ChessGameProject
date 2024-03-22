#pragma once

#include "GameEngine.h"

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

class Chessboard
{
	private:
		// Properties
		int player;
		int fields_size;

		// Update
		bool end_screen;
		bool quit;
		bool checkmate;
		bool figure_picked_up;
		bool make_move;
		FigureMove move_to;
		bool update_board;

		// Chessboard
		Field* chessboard[8][8];

		// Figures
		Figure* white_king;
		Figure* black_king;
		Figure* current_figure;
		SDL_Rect* last_collision;
		std::vector<Figure*> white_player;
		std::vector<Figure*> black_player;
		std::stack<Figure*> removed_figures;
		
		// End game
		Text white_won, black_won, pat, reset;

	public:
		static bool reset_game;
		static bool end_game;

		Chessboard(int fields_size);
		~Chessboard();

		// Create objects
		void CreateBoard();
		void CreateFigures();

		// Board features
		void DrawBoard();
		void DrawFigures();

		void BoardUpdate();
		void SwitchTurns();

		void UpdateFigures();
		void RenderFigures();

		// Figures features
		void MarkFieldsUnderAttack(std::vector<Figure*>& player_figures);
		void CalculateFigureMoves(std::vector<Figure*>& player_figures);
		void KingMechanic(std::vector<Figure*>& player_figures, std::vector<Figure*>& opposite_player_figures, Figure* king);
		void CheckForEntangling(std::vector<Figure*>& player_figures, Figure* opposite_king);
		void ApplyEntangledMoves(std::vector<Figure*>& player_figures);
		void EndGameConditions(std::vector<Figure*>& player_figures, Figure* king);
		void PickedUpFigure();
		void DrawMarksForMovesWhenPicked(std::vector<Figure*>& player_figures);
		void MoveFigure();

	private:
		Texture fields_colors[2] =
		{
			{ TextureMenager::LoadTexture("Textures/Chessboard/whiteSqr.png") },
			{ TextureMenager::LoadTexture("Textures/Chessboard/blackSqr.png") }
		};

		Texture marks[3] =
		{
			{ TextureMenager::LoadTexture("Textures/Chessboard/mark.png") },
			{ TextureMenager::LoadTexture("Textures/Chessboard/berth.png") },
			{ TextureMenager::LoadTexture("Textures/Chessboard/attack.png") }
		};
};

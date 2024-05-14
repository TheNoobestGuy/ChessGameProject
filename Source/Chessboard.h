#pragma once

#include "AI.h"

class Chessboard
{
	private:
		// Properties
		bool player_turn;
		bool computer_turn;
		bool computer_moved;
		bool first_turn;
		bool end_game;
		int fields_size;
		int player_score[2];
		int computer_score[2];

		// Computer
		AI* Computer;

		// Update
		bool end_screen;
		bool checkmate;
		bool no_moves[2];
		bool figure_picked_up;
		bool make_move;
		FigureMove move_to;
		bool update_board;

		// Chessboard
		Field* chessboard[8][8];

		// Figures
		Figure* player_king;
		Figure* computer_king;
		Figure* current_figure;
		Figure* figure_to_remove;
		SDL_Rect* last_collision;
		std::vector<Figure*> player_figures;
		std::vector<Figure*> computer_figures;
		
		// End game
		Text white_won, black_won, pat, reset;

		// Create objects
		void CreateBoard();
		void CreateFigures();

		// Board features
		void DrawBoard();
		void DrawFigures();

		// Figures features
		void EndGameCondition();
		void PickedUpFigure();
		void DrawMarksForMovesWhenPicked(std::vector<Figure*>& player_figures);
		void PickedUpDestination();
		void MoveFigure();

	public:
		Chessboard(int fields_size);
		~Chessboard();

		// Crucial functions
		void BoardUpdate();
		void AIComponent();
		void SwitchTurns();
		void UpdateFigures();
		void RenderFigures();
		void EndGame();

		// Properties
		int GetWhiteFiguresValue() { return player_score[0]; }
		int GetWhiteConqueredValue() { return player_score[1]; }
		int GetBlackFiguresValue() { return computer_score[0]; }
		int GetBlackConqueredValue() { return computer_score[1]; }

		std::vector<Figure*>& GetWhitePlayer() { return player_figures;  }
		std::vector<Figure*>& GetBlackPlayer() { return computer_figures;  }

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

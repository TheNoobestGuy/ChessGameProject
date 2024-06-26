#include "AI.h"

AI::AI()
{
	this->best_move = { {0, 0}, 0, 0};
	this->marked = nullptr;

	for (int i = 0; i < 6; i++)
	{
		chooseTopFigures[i] = new Text { "", {0, 0, 0}, GameEngine::CreateRectangle(0, 0, 64) };
		chooseBottomFigures[i] = new Text { "", {0, 0, 0}, GameEngine::CreateRectangle(0, 0, 64) };
	}

	int choice_window_bottom_figures = 64;
	int choice_window_top_figures = SCREEN_HEIGHT - 128;

	if (GameEngine::human_player == BLACK_FIGURES)
	{
		choice_window_bottom_figures = SCREEN_HEIGHT - 128;
		choice_window_top_figures = 64;
	}

	CreateChoiceWindow(WHITE_FIGURES, 64, 64, choice_window_bottom_figures, chooseBottomFigures);
	CreateChoiceWindow(BLACK_FIGURES, 64, 64, choice_window_top_figures, chooseTopFigures);
}

AI::~AI()
{
	for (int i = 0; i < 6; i++)
	{
		delete chooseTopFigures[i];
		chooseTopFigures[i] = nullptr;

		delete chooseBottomFigures[i];
		chooseBottomFigures[i] = nullptr;
	}

	marked = nullptr;
}

void AI::UpdateBoard(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Figure* player_king, Figure* computer_king, Figure* figure_to_remove, bool& checkmate)
{
	// Remove conquered figure from board
	RemoveFromBoard(figure_to_remove, player_figures, computer_figures);

	// Check does any pawn has become a queen
	HasBecomeFigure(chessboard, player_figures, computer_figures);

	// Attach positions of figures to a board
	AttachPositionsToBoard(chessboard, player_figures, computer_figures);

	// Calculate available moves for figures
	CalculateFigureMoves(chessboard, player_figures);
	CalculateFigureMoves(chessboard, computer_figures);

	// Mark fields under attack
	MarkFieldsUnderAttack(chessboard, player_figures);
	MarkFieldsUnderAttack(chessboard, computer_figures);

	// Check for entangling
	CheckForEntangling(chessboard, player_figures, computer_king);
	CheckForEntangling(chessboard, computer_figures, player_king);

	ApplyEntangledMoves(player_figures);
	ApplyEntangledMoves(computer_figures);

	// Kings mechanics
	KingMechanic(chessboard, player_figures, computer_figures, player_king, checkmate);
	KingMechanic(chessboard, computer_figures, player_figures, computer_king, checkmate);

	// Remove unavailable moves
	RemoveUnavailableMoves(player_figures);
	RemoveUnavailableMoves(computer_figures);

	player_king = nullptr;
	computer_king = nullptr;
	figure_to_remove = nullptr;
}

void AI::UpdateAI(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Figure* player_king, Figure* computer_king, Figure* figure_to_remove)
{
	best_move = FindBestMove(chessboard, 1, player_figures, computer_figures, player_king, computer_king, figure_to_remove);
}

int AI::MiniMaxAlphaBetaPrunning(Field* chessboard[][8], std::vector<std::tuple<Field, int>>& moves, int depth, int& alpha, int& beta, int maximazing_player, bool checkmate, std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Figure*& player_king, Figure*& computer_king, Figure*& figure_to_remove)
{
	if (depth == 0)
		return EvaluateBoard(chessboard, maximazing_player);

	if (maximazing_player == COMPUTER)
	{
		int max_eval = INT_MIN;
		for (Figure* figure : computer_figures)
		{
			for (Field_ID move : figure->available_moves)
			{
				// Determine an actual field of a figure and it move
				Field_ID previous_position = figure->GetField();
				Field field_of_move = { move, 64, 0 };
				field_of_move.figure = figure;

				figure->ChangePositionComputer(move);

				// Create new board that is essential for later updating figures possiblities
				Field* newChessboard[8][8];
				CreateNewChessboard(chessboard, newChessboard, field_of_move);

				// Create copy of a figures that are about to passed into update board function
				Figure* computer_king_update;
				std::vector<Figure*> computer_figures_update;
				MakeCopyOfFiguresForCalculatingMoves(computer_figures, computer_figures_update, computer_king_update);

				// Update board and push fruther a minimax algorithm
				if (depth != 1)
					UpdateBoard(newChessboard, player_figures, computer_figures_update, player_king, computer_king_update, figure_to_remove, checkmate);
				
				// Evaluate move
				int current_eval = MiniMaxAlphaBetaPrunning(newChessboard, moves, depth - 1, alpha, beta, HUMAN, checkmate, player_figures, computer_figures_update, player_king , computer_king_update, figure_to_remove);
				current_eval += EvaluateMove(chessboard, field_of_move, computer_figures_update, COMPUTER);

				figure->ChangePositionComputer(previous_position);

				// Clear memory that has been used
				DeleteCreatedChessboard(newChessboard);
				DeleteFigures(computer_figures_update);
				computer_king_update = nullptr;

				// Change value of a move
				if (current_eval > max_eval)
				{
					max_eval = current_eval;
				}

				// Alpha cut-off
				alpha = std::max(alpha, current_eval);
				if (beta <= alpha)
					break;
			}
		}

		return max_eval;
	}
	else
	{
		int min_eval = INT_MAX;
		for (Figure* figure : player_figures)
		{
			for (Field_ID move : figure->available_moves)
			{
				// Determine an actual field of a figure and it move
				Field_ID previous_position = figure->GetField();
				Field field_of_move = { move, 64, 0 };
				field_of_move.figure = figure;

				figure->ChangePositionComputer(move);

				// Create new board that is essential for later updating figures possiblities
				Field* newChessboard[8][8];
				CreateNewChessboard(chessboard, newChessboard, field_of_move);

				// Create copy of a figures that are about to passed into update board function
				Figure* player_king_update;
				std::vector<Figure*> player_figures_update;
				MakeCopyOfFiguresForCalculatingMoves(player_figures, player_figures_update, player_king_update);

				// Update board and push further a minimax algorithm
				if (depth != 1)
					UpdateBoard(newChessboard, player_figures_update, computer_figures, player_king_update, computer_king, figure_to_remove, checkmate);

				// Evaluate move
				int current_eval = MiniMaxAlphaBetaPrunning(newChessboard, moves, depth - 1, alpha, beta, COMPUTER, checkmate, player_figures_update, computer_figures, player_king_update, computer_king, figure_to_remove);
				current_eval -= EvaluateMove(chessboard, field_of_move, player_figures_update, HUMAN);

				figure->ChangePositionComputer(previous_position);

				// Clear memory that has been used
				DeleteCreatedChessboard(newChessboard);
				DeleteFigures(player_figures_update);
				player_king_update = nullptr;

				// Change value of a move
				if (current_eval < min_eval)
				{
					min_eval = current_eval;
				}

				// Beta cut-off
				beta = std::min(beta, current_eval);
				if (beta <= alpha)
					break;
			}
		}

		return min_eval;
	}
}

int AI::EvaluateMove(Field* chessboard[][8], Field& move, std::vector<Figure*>& player_figures, int player)
{
	int current_value = 0;

	// Get an opposite player and figure after update
	int opposite_player = HUMAN;
	if (player == HUMAN)
		opposite_player = COMPUTER;

	Figure* actual_figure = move.figure;
	for (Figure* figure : player_figures)
	{
		if (move.figure->GetID() == figure->GetID())
			actual_figure = figure;
	}

	// Check is there any checkmate
	if (!actual_figure->way_to_opposite_king.empty())
		current_value++;
	
	// Add some value depending on a conquered figure
	if (chessboard[move.field_ID.y][move.field_ID.x]->figure != nullptr)
		current_value += chessboard[move.field_ID.y][move.field_ID.x]->figure->GetValue();

	// Determine whether some field is under attack of an opposite player or not and subtract value of a potential lose
	if (chessboard[move.field_ID.y][move.field_ID.x]->field_under_attack[opposite_player])
		current_value -= move.figure->GetValue();

	// Add some value to a move depending on escaping from a capture of a figure
	if (chessboard[move.figure->GetField().y][move.figure->GetField().x]->field_under_attack[opposite_player])
		current_value += move.figure->GetValue();
	
	actual_figure = nullptr;

	return current_value;
}

int AI::EvaluateBoard(Field* chessboard[][8], int maximazing_player)
{	
	int player_value = 0;
	int computer_value = 0;

	// Check value of a board by counting a number of a figures of both players and add their values to their variables
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			if (chessboard[row][col]->figure != nullptr)
			{
				if (chessboard[row][col]->figure->GetPlayer() == COMPUTER)
					computer_value++;
				else
					player_value++;
			}
		}
	}

	return computer_value - player_value;
}

Field AI::FindBestMove(Field* chessboard[][8], int depth, std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Figure*& player_king, Figure*& computer_king, Figure*& figure_to_remove)
{
	std::vector<std::tuple<Field, int>> moves;
	bool checkmate = false;

	int alpha = INT_MIN;
	int beta = INT_MAX;
	for (Figure* figure : computer_figures)
	{
		for (Field_ID move : figure->available_moves)
		{
			// Push base move field into possible moves vector
			Field base_field_of_move = { move, 64, 0 };
			moves.push_back({ base_field_of_move, 0 });
			std::get<0>(moves.back()).figure = figure;

			// Save for later use starting position of a figure and determine move that is about to be check
			Field_ID previous_position = figure->GetField();
			Field field_of_move = { move, 64, 0 };
			field_of_move.figure = figure;

			figure->ChangePositionComputer(move);

			// Create new board basing on a actual move
			Field* newChessboard[8][8];
			CreateNewChessboard(chessboard, newChessboard, field_of_move);

			// Copy figures that are passed later to minimax algorithm
			Figure* computer_king_update;
			std::vector<Figure*> computer_figures_update;
			MakeCopyOfFiguresForCalculatingMoves(computer_figures, computer_figures_update, computer_king_update);

			// Update board and push further a minimax algorithm
			UpdateBoard(newChessboard, player_figures, computer_figures_update, player_king, computer_king_update, figure_to_remove, checkmate);

			// Evaluate move
			int current_eval = MiniMaxAlphaBetaPrunning(newChessboard, moves, depth, alpha, beta, HUMAN, checkmate, player_figures, computer_figures_update, player_king, computer_king_update, figure_to_remove);
			current_eval += EvaluateMove(chessboard, field_of_move, computer_figures_update, COMPUTER);

			figure->ChangePositionComputer(previous_position);

			// Clear memory that has been used and push value of a move to a vector
			DeleteCreatedChessboard(newChessboard);
			DeleteFigures(computer_figures_update);
			computer_king_update = nullptr;
			
			// Set value of a move that is depending on created tree by a minimax algorithm
			std::get<1>(moves.back()) = current_eval;
		}
	}
	
	// Find best moves
	std::vector<Field> best_moves;
	Field best_move;
	int buffor = INT_MIN;

	for (std::tuple<Field, int> move : moves)
	{
		if (std::get<1>(move) > buffor)
		{
			buffor = std::get<1>(move);
		}
	}

	for (std::tuple<Field, int> move : moves)
	{
		if (std::get<1>(move) >= buffor)
		{
			best_moves.push_back(std::get<0>(move));
		}
	}

	// Generate random value from given range for moves
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(best_moves.begin(), best_moves.end(), gen);

	// Randomly pick a move form a final vector of possiblilites
	std::uniform_int_distribution<> distribution(0, best_moves.size()-1);
	
	int random = distribution(gen);

	best_move = best_moves[random];

	return best_move;
}

void AI::CreateNewChessboard(Field* previousChessboard[][8], Field* newChessboard[][8], Field& move)
{
	// Create a new board
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			newChessboard[row][col] = new Field{ { col, row }, 64, 0 };
		}
	}

	// Copy a base board and append move that have to be check
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			newChessboard[row][col]->en_passant = previousChessboard[row][col]->en_passant;
			newChessboard[row][col]->field_under_attack[HUMAN] = previousChessboard[row][col]->field_under_attack[HUMAN];
			newChessboard[row][col]->field_under_attack[COMPUTER] = previousChessboard[row][col]->field_under_attack[COMPUTER];

			if (previousChessboard[row][col]->figure != nullptr)
			{
				if (previousChessboard[row][col]->figure->GetID() == move.figure->GetID())
				{
					if (previousChessboard[row][col]->figure->GetPlayer() == move.figure->GetPlayer())
					{
						newChessboard[row][col]->figure = nullptr;
					}
					else
					{
						newChessboard[row][col]->figure = previousChessboard[row][col]->figure;
					}
				}
				else if (previousChessboard[row][col]->field_ID == move.field_ID)
				{
					newChessboard[row][col]->figure = move.figure;
				}
				else
				{
					newChessboard[row][col]->figure = previousChessboard[row][col]->figure;
				}
			}
			else if(previousChessboard[row][col]->field_ID == move.field_ID)
			{
				newChessboard[row][col]->figure = move.figure;
			}
			else
			{
				newChessboard[row][col]->figure = nullptr;
			}
		}
	}
}

void AI::DeleteCreatedChessboard(Field* chessboard[][8])
{
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			delete chessboard[row][col];
			chessboard[row][col] = nullptr;
		}
	}
}

void AI::MakeCopyOfFiguresForCalculatingMoves(std::vector<Figure*>& player_figures, std::vector<Figure*>& player_figures_update, Figure*& player_king_update)
{
	for (Figure* figure : player_figures)
	{
		if (figure->GetName() == "Pawn")
			player_figures_update.push_back(new Pawn(figure->GetName(), figure->GetID(), figure->GetField(), figure->GetColor(), 64, figure->GetValue()));
		else if (figure->GetName() == "Knight")
			player_figures_update.push_back(new Knight(figure->GetName(), figure->GetID(), figure->GetField(), figure->GetColor(), 64, figure->GetValue()));
		else if (figure->GetName() == "Bishop")
			player_figures_update.push_back(new Bishop(figure->GetName(), figure->GetID(), figure->GetField(), figure->GetColor(), 64, figure->GetValue()));
		else if (figure->GetName() == "Rook")
			player_figures_update.push_back(new Rook(figure->GetName(), figure->GetID(), figure->GetField(), figure->GetColor(), 64, figure->GetValue()));
		else if (figure->GetName() == "Queen")
			player_figures_update.push_back(new Queen(figure->GetName(), figure->GetID(), figure->GetField(), figure->GetColor(), 64, figure->GetValue()));
		else if (figure->GetName() == "King")
		{
			player_figures_update.push_back(new King(figure->GetName(), figure->GetID(), figure->GetField(), figure->GetColor(), 64, figure->GetValue()));
			player_king_update = player_figures_update.back();
		}
		else
		{
			continue;
		}

		if (figure->GetPlayer() == COMPUTER)
			player_figures_update.back()->SetPlayer(COMPUTER);
		else
			player_figures_update.back()->SetPlayer(HUMAN);

		player_figures_update.back()->PossibleMoves();
	}
}

void AI::DeleteFigures(std::vector<Figure*>& player_figures)
{
	for (Figure* figure : player_figures)
	{
		delete figure;
		figure = nullptr;
	}
}

void AI::RemoveFromBoard(Figure* figure_to_remove, std::vector<Figure*>& player_figures,  std::vector<Figure*>& computer_figures)
{
	if (figure_to_remove != nullptr)
	{
		if (figure_to_remove->GetPlayer() == HUMAN)
		{
			for (int figure = 0; figure < player_figures.size(); figure++)
			{
				if (player_figures[figure]->GetID() == figure_to_remove->GetID())
				{
					delete player_figures[figure];
					player_figures[figure] = nullptr;
					player_figures.erase(player_figures.begin() + figure);
					break;
				}
			}
		}
		else
		{
			for (int figure = 0; figure < computer_figures.size(); figure++)
			{
				if (computer_figures[figure]->GetID() == figure_to_remove->GetID())
				{
					delete computer_figures[figure];
					computer_figures[figure] = nullptr;
					computer_figures.erase(computer_figures.begin() + figure);
					break;
				}
			}
		}
	}

	figure_to_remove = nullptr;
}

void AI::HasBecomeFigure(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures)
{
	int QueenBecomeField[2] = { 0, 7 };

	for (int figure = 0; figure < player_figures.size(); figure++)
	{
		if (player_figures[figure]->GetName() == "Pawn" && player_figures[figure]->GetField().y == QueenBecomeField[0])
		{
			// Window for choosing figure
			int selected_figure = 0;

			if (GameEngine::enemy != COMPUTER)
			{
				bool running = true;
				int choice = 0;

				ChoiceBetweenFigures(chessboard, player_figures, computer_figures, selected_figure, running, choice);
			}

			// Get position of pawn and delete it from board
			int tempID = player_figures[figure]->GetID();
			int tempColor = player_figures[figure]->GetColor();
			Field_ID tempField = player_figures[figure]->GetField();

			chessboard[tempField.y][tempField.x]->figure = nullptr;
			delete player_figures[figure];
			player_figures[figure] = nullptr;
			player_figures.erase(player_figures.begin() + figure);

			// Push new figure
			switch (selected_figure)
			{
			case 0:
				player_figures.push_back(new Queen("Queen", tempID, tempField, tempColor, 64, 9));
				break;

			case 1:
				player_figures.push_back(new Rook("Rook", tempID, tempField, tempColor, 64, 5));
				break;

			case 2:
				player_figures.push_back(new Knight("Knight", tempID, tempField, tempColor, 64, 3));
				break;

			case 3:
				player_figures.push_back(new Bishop("Bishop", tempID, tempField, tempColor, 64, 3));
				break;

			default:
				player_figures.push_back(new Queen("Queen", tempID, tempField, tempColor, 64, 9));
				break;
			}

			player_figures.back()->SetPlayer(HUMAN);
			player_figures.back()->PossibleMoves();
		}
	}

	for (int figure = 0; figure < computer_figures.size(); figure++)
	{
		if (computer_figures[figure]->GetName() == "Pawn" && computer_figures[figure]->GetField().y == QueenBecomeField[1])
		{
			// Window for choosing figure
			int selected_figure = 0;

			if (GameEngine::enemy != COMPUTER)
			{
				bool running = true;
				int choice = 0;

				ChoiceBetweenFigures(chessboard, player_figures, computer_figures, selected_figure, running, choice);
			}

			// Get position of pawn and delete it from board
			int tempID = computer_figures[figure]->GetID();
			int tempColor = computer_figures[figure]->GetColor();
			Field_ID tempField = computer_figures[figure]->GetField();

			chessboard[tempField.y][tempField.x]->figure = nullptr;
			delete computer_figures[figure];
			computer_figures[figure] = nullptr;
			computer_figures.erase(computer_figures.begin() + figure);

			// Push new figure
			switch (selected_figure)
			{
			case 0:
				computer_figures.push_back(new Queen("Queen", tempID, tempField, tempColor, 64, 9));
				break;

			case 1:
				computer_figures.push_back(new Rook("Rook", tempID, tempField, tempColor, 64, 5));
				break;

			case 2:
				computer_figures.push_back(new Knight("Knight", tempID, tempField, tempColor, 64, 3));
				break;

			case 3:
				computer_figures.push_back(new Bishop("Bishop", tempID, tempField, tempColor, 64, 3));
				break;

			default:
				computer_figures.push_back(new Queen("Queen", tempID, tempField, tempColor, 64, 9));
				break;
			}

			computer_figures.back()->SetPlayer(COMPUTER);
			computer_figures.back()->PossibleMoves();
		}
	}
}

void AI::CreateChoiceWindow(int figures_color, int start_field, int field, int field_y, Text* options[6])
{
	// Create a selection menu for choosing a figure when pawn reaches the opposite end of a board
	int field_x = start_field + field;

	for (int i = 1; i < 5; i++)
	{
		int buffor = i - 1;

		options[i]->rect.x = field_x;
		options[i]->rect.y = field_y;

		options[i]->text += buffor;
		options[i]->unselected = choiceFiguresUnselected[figures_color][buffor].texture;
		options[i]->selected = choiceFiguresSelected[figures_color][buffor].texture;

		field_x += field;
	}

	// Create border of a selection menu
	field_x = start_field;
	for (int i = 0, border = 0; i < 6; i += 5, border++)
	{
		options[i]->rect.x = field_x;
		options[i]->rect.y = field_y;

		options[i]->unselected = choiceBorders[border].texture;

		field_x = start_field + (field * 5);
	}
}

void AI::ChoiceBetweenFigures(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, int& selected_figure, bool& running, int& choice)
{
	marked = nullptr;
	Text* options[6];

	// Determine where and with which color a selection menu for choosing a figure should be displayed
	if (GameEngine::turn == COMPUTER)
	{
		if (GameEngine::human_player == WHITE_FIGURES)
		{
			for (int i = 0; i < 6; i++)
			{
				options[i] = chooseBottomFigures[i];
			}
		}
		else
		{
			for (int i = 0; i < 6; i++)
			{
				options[i] = chooseTopFigures[i];
			}
		}
	}
	else
	{
		if (GameEngine::human_player == WHITE_FIGURES)
		{
			for (int i = 0; i < 6; i++)
			{
				options[i] = chooseTopFigures[i];
			}
		}
		else
		{
			for (int i = 0; i < 6; i++)
			{
				options[i] = chooseBottomFigures[i];
			}
		}
	}

	while (running)
	{
		RenderChoice(chessboard, player_figures, computer_figures, options);
		UpdateChoice(options, selected_figure, running);
		EventHandlerForChoice(running, choice);
	}

	marked = nullptr;
	GameEngine::mouse_left = false;
}

void AI::UpdateChoice(Text* options[6], int& selected_figure, bool& running)
{
	// Mark and enable to choose an option from selection menu
	for (int select = 1; select < 5; select++)
	{
		if (marked == nullptr)
		{
			if (GameEngine::CollisionDetector(GameEngine::mouse_x, GameEngine::mouse_y, &options[select]->rect))
			{
				marked = options[select];
				marked->value = select - 1;
			}
		}
		else
		{
			if (!GameEngine::CollisionDetector(GameEngine::mouse_x, GameEngine::mouse_y, &marked->rect))
			{
				marked = nullptr;
			}
			else
			{
				if (GameEngine::mouse_left)
				{
					selected_figure = marked->value;

					GameEngine::mouse_left = false;
					marked = nullptr;
					running = false;
					break;
				}
			}
		}
	}
}

void AI::RenderChoice(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Text* options[6])
{
	SDL_RenderClear(GameEngine::renderer);

	// Render chessboard
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			if (chessboard[row][col]->color == BLACK)
				TextureMenager::Draw(fields_colors[1].texture, fields_colors[1].srcRect, chessboard[row][col]->field_rect);
			else
				TextureMenager::Draw(fields_colors[0].texture, fields_colors[0].srcRect, chessboard[row][col]->field_rect);
		}
	}

	for (Figure* figure : player_figures)
	{
		figure->Render();
	}

	for (Figure* figure : computer_figures)
	{
		figure->Render();
	}

	// Render option window
	for (int i = 0; i < 6; i++)
	{
		TextureMenager::Draw(options[i]->unselected, options[i]->rect);
	}

	// Render selected
	if (marked != nullptr)
	{
		TextureMenager::Draw(marked->selected, marked->rect);
	}

	SDL_RenderPresent(GameEngine::renderer);
}

void AI::EventHandlerForChoice(bool& running, int& choice)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	SDL_GetMouseState(&GameEngine::mouse_x, &GameEngine::mouse_y);

	switch (event.type)
	{
	case SDL_QUIT:
		running = false;
		GameEngine::isRunning = false;
		break;

	case SDL_MOUSEBUTTONDOWN:
		GameEngine::mouse_left = true;
		break;

	case SDL_MOUSEBUTTONUP:
		GameEngine::mouse_left = false;
		break;

	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE)
		{
			GameEngine::end_game = true;
		}
		break;

	default:
		break;
	}
}

void AI::AttachPositionsToBoard(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures)
{
	// Reset a chessboard
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			chessboard[row][col]->figure = nullptr;
			chessboard[col][row]->field_under_attack[0] = false;
			chessboard[col][row]->field_under_attack[1] = false;
			chessboard[col][row]->en_passant = false;
		}
	}

	// Append all figures that are already on a board
	for (Figure* figure : player_figures)
	{
		figure->Release();
		chessboard[figure->GetField().y][figure->GetField().x]->figure = figure;
	}

	for (Figure* figure : computer_figures)
	{
		figure->Release();
		chessboard[figure->GetField().y][figure->GetField().x]->figure = figure;
	}
}

void AI::CalculateFigureMoves(Field* chessboard[][8], std::vector<Figure*>& player_figures)
{
	for (Figure* figure : player_figures)
	{
		figure->available_moves.clear();

		// Calculate possible move for figures depending on their names and possibilities
		if (figure->GetName() == "King")
		{
			for (int move = 0; move < figure->moves_list.size(); move++)
			{
				// Desired move
				int move_x = figure->GetField().x + figure->moves_list[move].x;
				int move_y = figure->GetField().y + figure->moves_list[move].y;

				if ((move_x >= 0 && move_x < 8) && (move_y >= 0 && move_y < 8))
				{
					if (chessboard[move_y][move_x]->figure != nullptr)
					{
						if (chessboard[move_y][move_x]->figure->GetPlayer() != figure->GetPlayer())
						{
							figure->available_moves.push_back({ move_x, move_y });
						}
					}
					else
					{
						figure->available_moves.push_back({ move_x, move_y });
					}

					chessboard[move_y][move_x]->field_under_attack[figure->GetPlayer()] = true;
				}
			}
		}

		else if (figure->GetName() == "Queen" || figure->GetName() == "Rook" || figure->GetName() == "Bishop")
		{
			for (int move = 0; move < figure->moves_list.size(); move++)
			{
				bool next_axis = false;
				bool unavailable_moves = false;
				std::vector<Field_ID> way_to_king;
				way_to_king.push_back(figure->GetField());

				// Figure beginning position
				int move_x = figure->GetField().x + figure->moves_list[move].x;
				int move_y = figure->GetField().y + figure->moves_list[move].y;

				while (!next_axis)
				{
					// Sort moves on available and unavailable with keeping track of their axis at this moment for further discernion by a marking system later
					if ((move_x >= 0 && move_x < 8) && (move_y >= 0 && move_y < 8))
					{
						if (chessboard[move_y][move_x]->figure != nullptr)
						{
							if (chessboard[move_y][move_x]->figure->GetPlayer() != figure->GetPlayer())
							{
								if (unavailable_moves)
								{
									figure->available_moves.push_back({ move_x, move_y });
									figure->available_moves.back().move_axis = move;
									figure->available_moves.back().available_move = false;

									next_axis = true;
								}
								else
								{
									figure->available_moves.push_back({ move_x, move_y });
									figure->available_moves.back().move_axis = move;
									figure->available_moves.back().available_move = true;

									// If king is encountered append way that is leading to him to proper array
									if (chessboard[move_y][move_x]->figure->GetName() == "King")
									{
										figure->way_to_opposite_king = way_to_king;
										way_to_king.clear();

										figure->available_moves.back().available_move = false;
									}

									chessboard[move_y][move_x]->field_under_attack[figure->GetPlayer()] = true;
								}

								unavailable_moves = true;
							}
							// If friendly figure encountered append its position as last attack in axis
							else
							{
								chessboard[move_y][move_x]->field_under_attack[figure->GetPlayer()] = true;
								next_axis = true;
							}
						}
						else
						{
							if (unavailable_moves)
							{
								figure->available_moves.push_back({ move_x, move_y });
								figure->available_moves.back().move_axis = move;
								figure->available_moves.back().available_move = false;
							}
							else
							{
								figure->available_moves.push_back({ move_x, move_y });
								figure->available_moves.back().move_axis = move;
								figure->available_moves.back().available_move = true;

								way_to_king.push_back(figure->available_moves.back());
								chessboard[move_y][move_x]->field_under_attack[figure->GetPlayer()] = true;
							}
						}
					}
					// If move is out of board go to the next axis
					else
					{
						next_axis = true;
					}

					// Desired move
					move_x += figure->moves_list[move].x;
					move_y += figure->moves_list[move].y;
				}

				way_to_king.clear();
			}

		}

		else if (figure->GetName() == "Knight")
		{
			for (int move = 0; move < figure->moves_list.size(); move++)
			{
				// Desired move
				int move_x = figure->GetField().x + figure->moves_list[move].x;
				int move_y = figure->GetField().y + figure->moves_list[move].y;

				// Check for collisons with other figures
				if ((move_x >= 0 && move_x < 8) && (move_y >= 0 && move_y < 8))
				{
					if (chessboard[move_y][move_x]->figure != nullptr)
					{
						if (chessboard[move_y][move_x]->figure->GetPlayer() != figure->GetPlayer())
						{
							figure->available_moves.push_back({ move_x, move_y });

							// If king is encountered append way that is leading to him to proper array
							if (chessboard[move_y][move_x]->figure->GetName() == "King")
							{
								figure->way_to_opposite_king.push_back(figure->GetField());
								figure->way_to_opposite_king.push_back({ move_x, move_y });
							}
						}
					}
					else
					{
						figure->available_moves.push_back({ move_x, move_y });
					}

					chessboard[move_y][move_x]->field_under_attack[figure->GetPlayer()] = true;
				}
			}
		}

		else if (figure->GetName() == "Pawn")
		{
			// Desired move
			int move_x = figure->GetField().x + figure->moves_list[0].x;
			int move_y = figure->GetField().y + figure->moves_list[0].y;

			if ((move_x >= 0 && move_x < 8) && (move_y >= 0 && move_y < 8))
			{
				if (chessboard[move_y][move_x]->figure == nullptr)
				{
					figure->available_moves.push_back({ move_x, move_y });

					// First move special
					if (figure->IsItFirstMove())
					{
						move_x = figure->GetField().x + figure->moves_list[1].x;
						move_y = figure->GetField().y + figure->moves_list[1].y;

						if (chessboard[move_y][move_x]->figure == nullptr)
						{
							figure->available_moves.push_back({ move_x, move_y });
						}
					}
				}
			}

			// Attack
			for (int attack = 2; attack < 4; attack++)
			{
				int attack_x = figure->GetField().x + figure->moves_list[attack].x;
				int attack_y = figure->GetField().y + figure->moves_list[attack].y;

				if ((attack_x >= 0 && attack_x < 8) && (attack_y >= 0 && attack_y < 8))
				{
					if (chessboard[attack_y][attack_x]->figure != nullptr && chessboard[attack_y][attack_x]->figure->GetPlayer() != figure->GetPlayer())
					{
						figure->available_moves.push_back({ attack_x, attack_y });

						// If king is encountered append way that is leading to him to proper array
						if (chessboard[attack_y][attack_x]->figure->GetName() == "King")
						{
							figure->way_to_opposite_king.push_back(figure->GetField());
							figure->way_to_opposite_king.push_back({ attack_x, attack_y });
						}
					}

					chessboard[attack_y][attack_x]->field_under_attack[figure->GetPlayer()] = true;
				}
			}

			// En passant
			for (int en_passant = 4; en_passant < 6; en_passant++)
			{
				int en_passant_x = figure->GetField().x + figure->moves_list[en_passant].x;
				int en_passant_y = figure->GetField().y + figure->moves_list[en_passant].y;

				if ((en_passant_x >= 0 && en_passant_x < 8) && (en_passant_y >= 0 && en_passant_y < 8))
				{
					if (chessboard[en_passant_y][en_passant_x]->figure != nullptr && chessboard[en_passant_y][en_passant_x]->figure->EnPassant())
					{
						if (chessboard[en_passant_y][en_passant_x]->figure->GetName() == "Pawn" && chessboard[en_passant_y][en_passant_x]->figure->GetPlayer() != figure->GetPlayer())
						{
							int buffor = en_passant - 2;

							en_passant_x = figure->GetField().x + figure->moves_list[buffor].x;
							en_passant_y = figure->GetField().y + figure->moves_list[buffor].y;

							if (chessboard[en_passant_y][en_passant_x]->figure == nullptr)
							{
								figure->available_moves.push_back({ en_passant_x, en_passant_y });
								chessboard[en_passant_y][en_passant_x]->en_passant = true;
							}
						}
					}
				}
			}
		}
	}
}

void AI::MarkFieldsUnderAttack(Field* chessboard[][8], std::vector<Figure*>& player_figures)
{
	for (Figure* figure : player_figures)
	{
		int player = figure->GetPlayer();

		if (figure->GetName() == "Pawn")
		{
			// Static moves applier for Pawns
			int pos_x_1 = figure->GetField().x - 1;
			int pos_x_2 = figure->GetField().x + 1;
			int pos_y = figure->GetField().y;

			if (player == HUMAN)
			{
				pos_y--;
			}
			else if (player == COMPUTER)
			{
				pos_y++;
			}

			if (pos_y >= 0 && pos_y < 8)
			{
				if (pos_x_1 >= 0 && pos_x_1 < 8)
				{
					chessboard[pos_y][pos_x_1]->field_under_attack[player] = true;
				}
				if (pos_x_2 >= 0 && pos_x_2 < 8)
				{
					chessboard[pos_y][pos_x_2]->field_under_attack[player] = true;
				}
			}
		}
		else if (figure->GetName() == "King" || figure->GetName() == "Knight")
		{
			// Every possible moves of those figures are also attacks
			for (Field_ID attack : figure->available_moves)
			{
				chessboard[attack.y][attack.x]->field_under_attack[player] = true;
			}
		}
		else if (figure->GetName() == "Bishop" || figure->GetName() == "Rook" || figure->GetName() == "Queen")
		{
			// Check moves by their axis
			int axis = 0;
			bool figure_encountered = false;

			for (Field_ID attack : figure->available_moves)
			{
				if (attack.move_axis != axis)
				{
					axis = attack.move_axis;
					figure_encountered = false;
				}

				if (figure_encountered)
				{
					continue;
				}

				if (chessboard[attack.y][attack.x]->figure != nullptr)
				{
					if (chessboard[attack.y][attack.x]->figure->GetName() != "King")
					{
						figure_encountered = true;
					}

					chessboard[attack.y][attack.x]->field_under_attack[player] = true;
				}
				else
				{
					chessboard[attack.y][attack.x]->field_under_attack[player] = true;
				}
			}
		}
	}
}

void AI::CheckForEntangling(Field* chessboard[][8], std::vector<Figure*>& player_figures, Figure* opposite_king)
{
	for (Figure* figure : player_figures)
	{
		if (figure->GetName() != "King")
		{
			// Check every possible axis of move and entangle figure that is standing at way of attack on king
			for (int move_axis = 0; move_axis < 8; move_axis++)
			{
				Figure* entangled_figure = nullptr;
				std::vector<Field_ID> way_to_king;
				way_to_king.push_back({ figure->GetField().x, figure->GetField().y });
				int figures_counter = 0;

				for (Field_ID field : figure->available_moves)
				{
					if (field.move_axis == move_axis)
					{
						// Keep counter of encountering figure and skip every axis where there is more than one
						if (chessboard[field.y][field.x]->figure != nullptr)
						{
							if (figures_counter == 0)
							{
								if (chessboard[field.y][field.x]->figure->GetName() == "King")
								{
									break;
								}
								else
								{
									entangled_figure = chessboard[field.y][field.x]->figure;
									figures_counter++;
								}
							}
							else if (figures_counter >= 1)
							{
								// If there is encountered opposite king at the way push possible moves for entangled figure into an array of moves
								if (chessboard[field.y][field.x]->figure->GetName() == "King" && figure->GetPlayer() != chessboard[field.y][field.x]->figure->GetPlayer())
								{
									entangled_figure->MakeEntangled();

									for (Field_ID move : entangled_figure->available_moves)
									{
										for (Field_ID way : way_to_king)
										{
											if (move == way)
											{
												entangled_figure->entangled_moves.push_back(way);
											}
										}
									}
								}
								break;
							}
						}
						else
						{
							way_to_king.push_back(field);
							way_to_king.back().available_move = true;
						}
					}
				}

				entangled_figure = nullptr;
			}
		}
	}
}

void AI::ApplyEntangledMoves(std::vector<Figure*>& player_figures)
{
	for (Figure* figure : player_figures)
	{
		if (figure->IsItEntangled())
		{
			figure->available_moves = figure->entangled_moves;
		}
	}
}

void AI::KingMechanic(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& opposite_player_figures, Figure* king, bool& checkmate)
{
	int opposite_player = COMPUTER;

	if (king->GetPlayer() == COMPUTER)
	{
		opposite_player = HUMAN;
	}

	// Castling
	if (king->IsItFirstMove())
	{
		int rook_x_1 = 0;
		int rook_x_2 = 7;
		int rook_y = 0;

		if (king->GetPlayer() == HUMAN)
		{
			rook_y = 7;
		}

		if (!chessboard[king->GetField().y][king->GetField().x]->field_under_attack[opposite_player_figures[0]->GetPlayer()])
		{
			for (int move = 2; move < 4; move++)
			{
				int move_x = king->GetField().x;
				bool next_axis = false;

				while (!next_axis)
				{
					move_x += king->moves_list[move].x;

					// Checking is there any possible castling
					if (move_x >= 0 && move_x < 8)
					{
						if (chessboard[rook_y][move_x]->figure != nullptr)
						{
							if (chessboard[rook_y][move_x]->figure->GetName() == "Rook" && chessboard[rook_y][move_x]->figure->GetPlayer() == king->GetPlayer())
							{
								if (chessboard[rook_y][move_x]->figure->GetField().x == rook_x_1 || chessboard[rook_y][move_x]->figure->GetField().x == rook_x_2)
								{
									if (chessboard[rook_y][move_x]->figure->GetField().y == rook_y)
									{
										king->available_moves.push_back({ move_x, rook_y });
									}
								}
							}
							
							next_axis = true;
						}
					}
					else
					{
						next_axis = true;
					}
				}
			}
		}
	}

	// Remove unavailable moves from king pool of moves
	std::vector<Field_ID> buffor;

	for (Field_ID move : king->available_moves)
	{
		if (!chessboard[move.y][move.x]->field_under_attack[opposite_player])
		{
			buffor.push_back(move);
		}
	}
	king->available_moves = buffor;

	// Check is there a checkmate and calculate possible moves to save king if there are any
	if (chessboard[king->GetField().y][king->GetField().x]->field_under_attack[opposite_player])
	{
		checkmate = true;

		// Find common attacked fields to defend
		std::vector<Field_ID> common_fields;

		for (Figure* opposite_figure_1 : opposite_player_figures)
		{
			for (Field_ID attack_1 : opposite_figure_1->way_to_opposite_king)
			{
				for (Figure* opposite_figure_2 : opposite_player_figures)
				{
					if (opposite_figure_1->GetID() != opposite_figure_2->GetID())
					{
						for (Field_ID attack_2 : opposite_figure_2->way_to_opposite_king)
						{
							if (attack_1 == attack_2)
							{
								common_fields.push_back(attack_1);
							}
						}
					}
				}
			}

			if (common_fields.empty())
			{
				common_fields = opposite_figure_1->way_to_opposite_king;
			}
		}

		// Find if there is possible defence by move of figures
		for (Figure* figure : player_figures)
		{
			if (figure->GetName() == "Pawn")
			{
				buffor.clear();

				for (Field_ID defence : figure->available_moves)
				{
					for (Field_ID attack : common_fields)
					{
						if (chessboard[defence.y][defence.x]->figure != nullptr && defence.x == figure->GetField().x)
						{
							continue;
						}
						else
						{
							if (defence == attack)
							{
								buffor.push_back(defence);
							}
						}
					}
				}

				figure->available_moves = buffor;
			}

			else if (figure->GetName() != "King")
			{
				buffor.clear();

				for (Field_ID defence : figure->available_moves)
				{
					for (Field_ID attack : common_fields)
					{
						if (defence == attack)
						{
							buffor.push_back(defence);
						}
					}
				}

				figure->available_moves = buffor;
			}
		}
	}
}

void AI::RemoveUnavailableMoves(std::vector<Figure*>& player_figures)
{
	for (Figure* figure : player_figures)
	{
		if (figure->GetName() == "Queen" || figure->GetName() == "Rook" || figure->GetName() == "Bishop")
		{
			for (int move = 0; move < figure->available_moves.size(); move++)
			{
				if (figure->available_moves[move].available_move == false)
				{
					figure->available_moves.erase(figure->available_moves.begin() + move);
					move--;
				}
			}
		}
	}
}
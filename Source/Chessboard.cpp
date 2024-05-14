#include "Chessboard.h"

Chessboard::Chessboard(int fields_size)
{
	// Properties
	if (GameEngine::human_player == WHITE_FIGURES)
	{
		this->player_turn = true;
		this->computer_turn = false;
	}
	else
	{
		this->player_turn = false;
		this->computer_turn = true;
	}
	
	this->first_turn = true;
	this->end_game = false;
	this->computer_moved = false;
	this->fields_size = fields_size;

	// Update
	this->end_screen = false;
	this->checkmate = false;
	this->no_moves[0] = true;
	this->no_moves[1] = true;
	this->figure_picked_up = false;
	this->make_move = false;
	this->move_to = { 0, 0 };
	this->update_board = true;

	this->player_score[0] = 0;
	this->player_score[1] = 0;
	this->computer_score[0] = 0;
	this->computer_score[1] = 0;

	// Figures
	this->player_king = nullptr;
	this->computer_king = nullptr;
	this->current_figure = nullptr;
	this->last_collision = nullptr;
	this->figure_to_remove = nullptr;

	// Create chessboard
	CreateBoard();
	CreateFigures();

	// Computer
	Computer = new AI();

	// Text
	white_won = { "White player won", {0, 0, 0}, GameEngine::CreateRectangle(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 100, 300, 100)};
	white_won.unselected = TextureMenager::LoadFont(white_won.text, white_won.color);

	black_won = { "Black player won", {0, 0, 0}, GameEngine::CreateRectangle(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 100, 300, 100) };
	black_won.unselected = TextureMenager::LoadFont(black_won.text, black_won.color);

	pat = { "Pat", {0, 0, 0}, GameEngine::CreateRectangle(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 100, 300, 100) };
	pat.unselected = TextureMenager::LoadFont(pat.text, pat.color);

	reset = { "Press any key to go back to main menu or mouse to reset game", {0, 0, 0}, GameEngine::CreateRectangle(SCREEN_WIDTH/2 - 250, SCREEN_HEIGHT/2, 500, 50) };
	reset.unselected = TextureMenager::LoadFont(reset.text, reset.color);
}

Chessboard::~Chessboard() 
{
	player_king = nullptr;
	computer_king = nullptr;
	current_figure = nullptr;
	last_collision = nullptr;
	figure_to_remove = nullptr;

	delete Computer;
	Computer = nullptr;

	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			delete chessboard[row][col];
			chessboard[row][col] = nullptr;
		}
	}
	for (Figure* figure : player_figures)
	{
		delete figure;
		figure = nullptr;
	}
	for (Figure* figure : computer_figures)
	{
		delete figure;
		figure = nullptr;
	}
}

void Chessboard::CreateBoard()
{
	int color = 0;

	for (int row = 0; row < 8; row++)
	{
		if (row % 2 == 0)
			color = WHITE;
		else
			color = BLACK;

		for (int col = 0; col < 8; col++)
		{
			if (color == BLACK)
			{
				chessboard[row][col] = new Field{ { col, row }, fields_size, color};
				color = WHITE;
			}
			else
			{
				chessboard[row][col] = new Field { { col, row }, fields_size, color};
				color = BLACK;
			}
		}
	}
}

void Chessboard::CreateFigures()
{
	int ID = 1;

	int player_row[2] = { 6, 7 };
	int computer_row[2] = { 1, 0 };

	int figures_colors[2] = { WHITE, BLACK };

	if (GameEngine::human_player == BLACK_FIGURES)
	{
		figures_colors[0] = BLACK;
		figures_colors[1] = WHITE;
	}

	// Pawns
	std::string name = "Pawn";
	for (int i = 0; i < 8; i++, ID++)
	{
		player_figures.push_back(new Pawn(name, ID, chessboard[player_row[0]][i]->field_ID, figures_colors[0], 64, 1));
		computer_figures.push_back(new Pawn(name, ID, chessboard[computer_row[0]][i]->field_ID, figures_colors[1], 64, 1));
	}


	// Knights
	name = "Knight";
	for (int i = 1; i < 8; i += 5, ID++)
	{
		player_figures.push_back(new Knight(name, ID, chessboard[player_row[1]][i]->field_ID, figures_colors[0], 64, 3));
		computer_figures.push_back(new Knight(name, ID, chessboard[computer_row[1]][i]->field_ID, figures_colors[1], 64, 3));
	}

	// Bishops
	name = "Bishop";
	for (int i = 2; i < 8; i += 3, ID++)
	{
		player_figures.push_back(new Bishop(name, ID, chessboard[player_row[1]][i]->field_ID, figures_colors[0], 64, 3));
		computer_figures.push_back(new Bishop(name, ID, chessboard[computer_row[1]][i]->field_ID, figures_colors[1], 64, 3));
	}

	// Rooks
	name = "Rook";
	for (int i = 0; i < 8; i += 7, ID++)
	{
		player_figures.push_back(new Rook(name, ID, chessboard[player_row[1]][i]->field_ID, figures_colors[0], 64, 5));
		computer_figures.push_back(new Rook(name, ID, chessboard[computer_row[1]][i]->field_ID, figures_colors[1], 64, 5));
	}

	// Queens
	name = "Queen";
	player_figures.push_back(new Queen(name, ID, chessboard[player_row[1]][3]->field_ID, figures_colors[0], 64, 9));
	computer_figures.push_back(new Queen(name, ID, chessboard[computer_row[1]][3]->field_ID, figures_colors[1], 64, 9));

	// Kings
	name = "King";
	ID++;
	player_figures.push_back(new King(name, ID, chessboard[player_row[1]][4]->field_ID, figures_colors[0], 64, 0));
	player_king = player_figures.back();

	computer_figures.push_back(new King(name, ID, chessboard[computer_row[1]][4]->field_ID, figures_colors[1], 64, 0));
	computer_king = computer_figures.back();

	// Append moves and set player type
	for (Figure* figure : player_figures)
	{
		figure->SetPlayer(HUMAN);
		figure->PossibleMoves();
	}
	for (Figure* figure : computer_figures)
	{
		figure->SetPlayer(COMPUTER);
		figure->PossibleMoves();
	}
}

void Chessboard::DrawBoard()
{
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
}

void Chessboard::DrawFigures()
{
	for (Figure* figure : player_figures)
	{
		figure->Render();
	}

	for (Figure* figure : computer_figures)
	{
		figure->Render();
	}
}

void Chessboard::BoardUpdate()
{
	if (update_board)
	{
		checkmate = false;

		// Calculate positions on board
		Computer->UpdateBoard(chessboard, player_figures, computer_figures, player_king, computer_king, figure_to_remove, checkmate);

		// End game condition check
		EndGameCondition();

		update_board = false;

		if ((no_moves[0] || no_moves[1]) && checkmate)
			end_game = true;
	}
}

void Chessboard::AIComponent()
{
	if (GameEngine::enemy == COMPUTER)
	{
		if (computer_turn && !end_game)
		{
			Computer->UpdateAI(chessboard, player_figures, computer_figures, player_king, computer_king, figure_to_remove);

			current_figure = Computer->MoveFigure();

			if (current_figure != nullptr)
			{
				move_to.x = Computer->MoveToField().x;
				move_to.y = Computer->MoveToField().y;

				if (chessboard[move_to.y][move_to.x]->figure != nullptr)
					move_to.attacked_figure = chessboard[move_to.y][move_to.x]->figure;
			}

			make_move = true;
			update_board = true;
			computer_moved = true;
		}
	}
}

void Chessboard::SwitchTurns()
{
	if (update_board)
	{
		if (player_turn)
		{
			player_turn = false;
			computer_turn = true;
		}
		else
		{
			player_turn = true;
			computer_turn = false;
		}

		computer_moved = false;
	}
}

void Chessboard::UpdateFigures()
{
	if (!end_game)
	{
		if (player_turn)
		{
			PickedUpFigure();

			for (Figure* figure : player_figures)
			{
				figure->PickUp(figure_picked_up);
			}

			PickedUpDestination();
		}
		else if (computer_turn && GameEngine::enemy == HUMAN)
		{
			PickedUpFigure();

			for (Figure* figure : computer_figures)
			{
				figure->PickUp(figure_picked_up);
			}

			PickedUpDestination();
		}
		
		MoveFigure();
	}
}

void Chessboard::RenderFigures()
{
	if (!end_game)
	{
		SDL_RenderClear(GameEngine::renderer);

		DrawBoard();

		if (GameEngine::enemy == HUMAN && computer_turn)
		{
			DrawMarksForMovesWhenPicked(computer_figures, player_figures);
		}
		else
		{
			DrawMarksForMovesWhenPicked(player_figures, computer_figures);
		}

		DrawFigures();

		// Render the picked up figure in front
		if (current_figure != nullptr)
			current_figure->Render();

		SDL_RenderPresent(GameEngine::renderer);
	}
}

void Chessboard::EndGameCondition()
{
	no_moves[0] = true;
	no_moves[1] = true;

	for (Figure* figure : player_figures)
	{
		if (!figure->available_moves.empty())
		{
			no_moves[0] = false;
			break;
		}
	}

	for (Figure* figure : computer_figures)
	{
		if (!figure->available_moves.empty())
		{
			no_moves[1] = false;
			break;
		}
	}
}

void Chessboard::EndGame()
{
	if ((update_board || end_game) && !computer_moved)
	{
		// Win by checkmate
		if ((no_moves[0] || no_moves[1]) && checkmate)
		{
			update_board = false;
			end_screen = true;

			while (end_screen)
			{
				SDL_RenderClear(GameEngine::renderer);

				DrawBoard();
				DrawFigures();

				if ((computer_turn && GameEngine::human_player == WHITE_FIGURES) || (player_turn && GameEngine::human_player == BLACK_FIGURES))
				{
					TextureMenager::Draw(white_won.unselected, white_won.rect);
				}
				else
				{
					TextureMenager::Draw(black_won.unselected, black_won.rect);
				}

				TextureMenager::Draw(reset.unselected, reset.rect);

				SDL_RenderPresent(GameEngine::renderer);

				SDL_Event event;
				SDL_PollEvent(&event);

				switch (event.type)
				{
				case SDL_QUIT:
					end_screen = false;
					GameEngine::isRunning = false;
					break;

				case SDL_KEYDOWN:
					GameEngine::end_game = true;
					end_screen = false;
					break;

				case SDL_MOUSEBUTTONDOWN:
					GameEngine::reset_game = true;
					end_screen = false;
					break;

				default:
					break;
				}
			}

			GameEngine::mouse_left = false;
		}
		// Pat
		else if ((no_moves[0] || no_moves[1]))
		{
			update_board = false;
			end_screen = true;

			while (end_screen)
			{
				SDL_RenderClear(GameEngine::renderer);

				DrawBoard();
				DrawFigures();

				TextureMenager::Draw(pat.unselected, pat.rect);

				TextureMenager::Draw(reset.unselected, reset.rect);

				SDL_RenderPresent(GameEngine::renderer);

				SDL_Event event;
				SDL_PollEvent(&event);

				switch (event.type)
				{
				case SDL_QUIT:
					end_screen = false;
					GameEngine::isRunning = false;
					break;

				case SDL_KEYDOWN:
					GameEngine::end_game = true;
					end_screen = false;
					break;

				case SDL_MOUSEBUTTONDOWN:
					GameEngine::reset_game = true;
					end_screen = false;
					break;

				default:
					break;
				}
			}

			GameEngine::mouse_left = false;
		}
	}

	if (GameEngine::end_game)
	{
		GameEngine::stage = 0;
		GameEngine::end_game = false;
	}
	else if (GameEngine::reset_game)
	{
		GameEngine::initialize_stage = true;
		GameEngine::reset_game = false;
	}
}

void Chessboard::PickedUpFigure()
{
	current_figure = nullptr;

	if (GameEngine::enemy == HUMAN && computer_turn)
	{
		for (Figure* figure : computer_figures)
		{
			if (figure->PickedUp())
			{
				current_figure = figure;
				break;
			}
		}
	}
	else
	{
		for (Figure* figure : player_figures)
		{
			if (figure->PickedUp())
			{
				current_figure = figure;
				break;
			}
		}
	}
}

void Chessboard::DrawMarksForMovesWhenPicked(std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures)
{
	for (Figure* figure : player_figures)
	{
		if (figure->PickedUp())
		{
			// Mark for place from where figure was taken 
			SDL_Rect static_rect = GameEngine::CreateRectangle(figure->GetField().x, figure->GetField().y, fields_size);
			TextureMenager::Draw(marks[1].texture, marks[1].srcRect, static_rect);

			// Marks for available moves
			for (Field_ID field : figure->available_moves)
			{
				if (field.available_move)
				{
					bool attack = false;

					for (Figure* figure : computer_figures)
					{
						if (field == figure->GetField())
						{
							attack = true;
						}
					}

					SDL_Rect possble_move = GameEngine::CreateRectangle(field.x, field.y, fields_size);

					if (attack)
					{
						TextureMenager::Draw(marks[2].texture, marks[2].srcRect, possble_move);
					}
					else
					{
						TextureMenager::Draw(marks[0].texture, marks[0].srcRect, possble_move);
					}
				}
			}
		}
	}
}

void Chessboard::PickedUpDestination()
{
	if (current_figure != nullptr)
	{
		// Check for collision with board fields while figure raised up
		if (current_figure->PickedUp())
		{
			for (int move = 0; move < current_figure->available_moves.size(); move++)
			{
				if (current_figure->available_moves[move].available_move)
				{
					if (GameEngine::CollisionDetector(current_figure->GetMotionRect(), &chessboard[current_figure->available_moves[move].y][current_figure->available_moves[move].x]->field_rect))
					{
						if (!make_move)
						{
							move_to.x = current_figure->available_moves[move].x;
							move_to.y = current_figure->available_moves[move].y;
							move_to.attacked_figure = chessboard[current_figure->available_moves[move].y][current_figure->available_moves[move].x]->figure;

							make_move = true;
							last_collision = &chessboard[current_figure->available_moves[move].y][current_figure->available_moves[move].x]->field_rect;
						}
					}
					else if (last_collision != nullptr)
					{
						if (!GameEngine::CollisionDetector(current_figure->GetMotionRect(), last_collision))
						{
							move_to.x = 0;
							move_to.y = 0;
							move_to.attacked_figure = nullptr;

							make_move = false;
							last_collision = nullptr;
						}
					}
				}
			}
		}
	}
}

void Chessboard::MoveFigure()
{
	if (current_figure != nullptr)
	{
		if (first_turn && computer_turn && GameEngine::enemy == COMPUTER)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(400));
			first_turn = false;
		}

		// Send new field ID to figure and check if there is any collision with some other figures
		if (make_move && !current_figure->PickedUp())
		{
			// Attack			
			if (move_to.attacked_figure != nullptr && move_to.attacked_figure->GetPlayer() != current_figure->GetPlayer())
			{
				if (current_figure->GetPlayer() == HUMAN)
				{
					for (Figure* figure : computer_figures)
					{
						if (figure->GetID() == move_to.attacked_figure->GetID())
						{
							figure_to_remove = figure;
							break;
						}
					}
				}
				else
				{
					for (Figure* figure : player_figures)
					{
						if (figure->GetID() == move_to.attacked_figure->GetID())
						{
							figure_to_remove = figure;
							break;
						}
					}
				}
			}
			// En passant
			if (current_figure->GetName() == "Pawn")
			{
				if (move_to.attacked_figure == nullptr && chessboard[move_to.y][move_to.x]->en_passant == true)
				{
					if (current_figure->GetPlayer() == COMPUTER)
					{
						figure_to_remove = chessboard[move_to.y - 1][move_to.x]->figure;
					}
					else
					{
						figure_to_remove = chessboard[move_to.y + 1][move_to.x]->figure;
					}
				}
				else if (move_to.attacked_figure == nullptr)
				{
					if (abs(move_to.y - current_figure->GetField().y) == 2)
					{
						current_figure->EnPassantVulnerablity();
					}
				}
			}

			// Make move
			current_figure->ChangePosition({ move_to.x, move_to.y });
			update_board = true;
			make_move = false;
			current_figure = nullptr;
			move_to.attacked_figure = nullptr;
			first_turn = false;

			if (computer_turn && GameEngine::enemy == COMPUTER)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(400));
				computer_turn = false;
			}
		}
	}
}

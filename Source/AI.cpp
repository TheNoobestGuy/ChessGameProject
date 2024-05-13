#include "AI.h"

AI::AI()
{
	this->best_move = { {0, 0}, 0, 0};
}

AI::~AI()
{
}

void AI::UpdateBoard(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Figure* player_king, Figure* computer_king, Figure* figure_to_remove, bool& checkmate)
{
	// Remove conquered figure from board
	RemoveFromBoard(figure_to_remove, player_figures, computer_figures);

	// Check does any pawn has become a queen
	HasBecomeQueen(player_figures, computer_figures);

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
}

void AI::UpdateAI(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures, Figure* player_king, Figure* computer_king, Figure* figure_to_remove)
{
	best_move = FindBestMove(chessboard, player_figures, computer_figures, player_king, computer_king, figure_to_remove, 2);
}

int AI::EvaluateBoard(Field* chessboard[][8], Field& move, bool computer_turn)
{
	int value = 0;
	bool exit = false;

	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			if (chessboard[row][col]->figure != nullptr)
			{
				if (chessboard[row][col]->figure->GetField() == move.field_ID)
				{
					if (chessboard[row][col]->figure->GetPlayer() == HUMAN)
					{
						value += chessboard[row][col]->figure->GetValue();
						exit = true;
						break;
					}
				}
			}
		}

		if (exit)
		{
			break;
		}
	}

	return value;
}

void AI::EvaluatingMovesAlgorithm(Field* chessboard[][8], Field& base_move, Field& move, std::vector<std::tuple<int, Field>>& moves, std::vector<Figure*> player_figures, std::vector<Figure*> computer_figures, Figure* player_king, Figure* computer_king, Figure* figure_to_remove, bool& computer_turn, bool checkmate, int& value, int depth)
{
	if (depth == 0)
	{
		moves.push_back({ value, base_move });
		return;
	}

	for (Field_ID field : move.figure->available_moves)
	{
		CheckMove(chessboard, newChessboard, move);

		UpdateBoard(newChessboard, player_figures, computer_figures, player_king, computer_king, figure_to_remove, checkmate);

		value += EvaluateBoard(newChessboard, move, computer_turn);

		EvaluatingMovesAlgorithm(newChessboard, base_move, move, moves, player_figures, computer_figures, player_king, computer_king, figure_to_remove, computer_turn, checkmate, value, depth - 1);
	}
}

Field AI::FindBestMove(Field* chessboard[][8], std::vector<Figure*> player_figures, std::vector<Figure*> computer_figures, Figure* player_king, Figure* computer_king, Figure* figure_to_remove, int depth)
{
	std::vector<std::tuple<int, Field>> moves;

	for (Figure* figure : computer_figures)
	{
		for (Field_ID field : figure->available_moves)
		{
			bool computer_turn = true;
			bool checkmate = false;

			Field move = { field, 0, 0 };
			move.figure = figure;

			int value = EvaluateBoard(chessboard, move, computer_turn);

			EvaluatingMovesAlgorithm(chessboard, move, move, moves, player_figures, computer_figures, player_king, computer_king, figure_to_remove, computer_turn, checkmate, value, depth - 1);
		}
	}

	std::vector<Field> best_moves;
	Field best_move;
	int buffor = INT_MIN;

	for (std::tuple<int, Field> move : moves)
	{
		if (std::get<0>(move) > buffor)
		{
			buffor = std::get<0>(move);
		}
	}

	for (std::tuple<int, Field> move : moves)
	{
		//std::cout << std::endl;
		//std::cout << "Value: " << std::get<0>(move) << ", Field: " << std::get<1>(move).field_ID.x << ", " << std::get<1>(move).field_ID.y << std::endl;
		if (std::get<0>(move) >= buffor)
		{
			buffor = std::get<0>(move);
			best_move = std::get<1>(move);

			best_moves.push_back(best_move);
		}
	}

	// Generate random value from given range for moves
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<> distribution(0, best_moves.size()-1);
	
	int random = distribution(gen);

	best_move = best_moves[random];

	return best_move;
}

void AI::CheckMove(Field* chessboard[][8], Field* newChessboard[][8], Field& move)
{
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			newChessboard[row][col] = chessboard[row][col];
		}
	}
	
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			if (newChessboard[row][col]->figure != nullptr)
			{
				if (newChessboard[row][col]->figure->GetID() == move.figure->GetID())
				{
					if (newChessboard[row][col]->figure->GetPlayer() == move.figure->GetPlayer())
					{
						newChessboard[row][col]->figure = nullptr;
					}
				}
			}

			if (newChessboard[row][col]->field_ID == move.field_ID)
			{
				newChessboard[row][col]->figure = move.figure;
			}
		}
	}
}

void AI::deleteChessboard(Field* chessboard[][8])
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

					figure_to_remove = nullptr;
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

					figure_to_remove = nullptr;
					break;
				}
			}
		}
	}
}

void AI::HasBecomeQueen(std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures)
{
	int QueenBecomeField[2] = { 0, 7 };

	for (int figure = 0; figure < player_figures.size(); figure++)
	{
		if (player_figures[figure]->GetName() == "Pawn" && player_figures[figure]->GetField().y == QueenBecomeField[0])
		{
			int tempID = player_figures[figure]->GetID();
			int tempColor = player_figures[figure]->GetColor();
			Field_ID tempField = player_figures[figure]->GetField();

			delete player_figures[figure];
			player_figures[figure] = nullptr;
			player_figures.erase(player_figures.begin() + figure);

			player_figures.push_back(new Queen("Queen", tempID, tempField, tempColor, 64, 9));
			player_figures.back()->SetPlayer(HUMAN);
			player_figures.back()->PossibleMoves();
		}
	}

	for (int figure = 0; figure < computer_figures.size(); figure++)
	{
		if (computer_figures[figure]->GetName() == "Pawn" && computer_figures[figure]->GetField().y == QueenBecomeField[1])
		{
			int tempID = computer_figures[figure]->GetID();
			int tempColor = computer_figures[figure]->GetColor();
			Field_ID tempField = computer_figures[figure]->GetField();

			delete computer_figures[figure];
			computer_figures[figure] = nullptr;
			computer_figures.erase(computer_figures.begin() + figure);

			computer_figures.push_back(new Queen("Queen", tempID, tempField, tempColor, 64, 9));
			computer_figures.back()->SetPlayer(COMPUTER);
			computer_figures.back()->PossibleMoves();
		}
	}
}

void AI::AttachPositionsToBoard(Field* chessboard[][8], std::vector<Figure*>& player_figures, std::vector<Figure*>& computer_figures)
{
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
		// Clear unactual available moves
		figure->available_moves.clear();

		// Calculate possible move for figures
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
						else
						{
							chessboard[move_y][move_x]->field_under_attack[figure->GetPlayer()] = true;
						}
					}
					else
					{
						figure->available_moves.push_back({ move_x, move_y });
					}
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
						else
						{
							chessboard[move_y][move_x]->field_under_attack[figure->GetPlayer()] = true;
						}
					}
					else
					{
						figure->available_moves.push_back({ move_x, move_y });
					}
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
					else if (chessboard[attack_y][attack_x]->figure != nullptr && chessboard[attack_y][attack_x]->figure->GetPlayer() == figure->GetPlayer())
					{
						chessboard[attack_y][attack_x]->field_under_attack[figure->GetPlayer()] = true;
					}
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
		opposite_player = HUMAN;

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
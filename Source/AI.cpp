#include "AI.h"

AI::AI(Field* chessboard[][8])
{
	this->best_move = { { 0, 0 }, nullptr };
	this->converted_chessboard = new AI_Field*[8];

	for (int row = 0; row < 8; row++)
	{
		this->converted_chessboard[row] = new AI_Field[8];

		for (int col = 0; col < 8; col++)
		{
			this->converted_chessboard[row][col].field_ID = chessboard[row][col]->field_ID;
			this->converted_chessboard[row][col].figure = chessboard[row][col]->figure;
		}
	}
}

AI::~AI() 
{
	deleteChessboard(converted_chessboard);
}

void AI::UpdateAI(Field* chessboard[][8], std::vector<Figure*>& computer_figures)
{
	AttachAvailableMoves(computer_figures);

	if (!available_moves.empty())
	{
		ConvertBoard(chessboard);

		best_move = FindBestMove(converted_chessboard, 3);
	}
	else
	{
		best_move = { { 0, 0 }, nullptr };
	}
}

void AI::AttachAvailableMoves(std::vector<Figure*>& computer_figures)
{
	available_moves.clear();

	for (Figure* figure : computer_figures)
	{
		for (Field_ID field : figure->available_moves)
		{
			available_moves.push_back({ field, figure });
		}
	}
}

void AI::ConvertBoard(Field* chessboard[][8])
{
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			converted_chessboard[row][col].field_ID = chessboard[row][col]->field_ID;
			converted_chessboard[row][col].figure = chessboard[row][col]->figure;
		}
	}
}

int AI::EvaluateBoard(AI_Field** chessboard)
{
	int value = 0;

	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			if (chessboard[row][col].figure == nullptr)
				value++;
		}
	}

	return value;
}

int AI::MiniMax(AI_Field** chessboard, int depth, int alpha, int beta, bool maximizing_player)
{
	if (depth == 0)
		return EvaluateBoard(chessboard);

	if (maximizing_player)
	{
		int max_eval = INT_MIN;

		for (AI_Field move : available_moves)
		{
			AI_Field** newChessboard = CheckMove(chessboard, move);
			int eval = MiniMax(newChessboard, depth - 1, alpha, beta, maximizing_player);
			max_eval = std::max(max_eval, eval);
			alpha = std::max(alpha, eval);

			deleteChessboard(newChessboard);

			if (beta <= alpha)
			{
				break;
			}
		}

		return max_eval;
	}
	else
	{
		int min_eval = INT_MAX;

		for (AI_Field move : available_moves)
		{
			AI_Field** newChessboard = CheckMove(chessboard, move);
			int eval = MiniMax(newChessboard, depth - 1, alpha, beta, maximizing_player);
			min_eval = std::min(min_eval, eval);
			alpha = std::min(beta, eval);

			deleteChessboard(newChessboard);

			if (beta <= alpha)
			{
				break;
			}
		}

		return min_eval;
	}
}

AI_Field AI::FindBestMove(AI_Field** chessboard, int depth)
{
	AI_Field best_move;

	int max_eval = INT_MIN;
	int alpha = INT_MIN;
	int beta = INT_MAX;

	for (AI_Field move : available_moves)
	{
		AI_Field** newChessboard = CheckMove(chessboard, move);
		int eval = MiniMax(newChessboard, depth - 1, alpha, beta, false);
		
		if (eval > max_eval)
		{
			max_eval = eval;
			best_move = move;
		}

		deleteChessboard(newChessboard);
	}

	return best_move;
}

AI_Field** AI::CheckMove(AI_Field** chessboard, AI_Field& move)
{
	AI_Field** newChessboard = new AI_Field*[8];

	for (int row = 0; row < 8; row++)
	{
		newChessboard[row] = new AI_Field[8];

		for (int col = 0; col < 8; col++)
		{
			if (chessboard[row][col].figure == move.figure)
			{
				newChessboard[row][col].field_ID = chessboard[row][col].field_ID;
				newChessboard[row][col].figure = nullptr;
			}
			else if (chessboard[row][col].field_ID == move.field_ID)
			{
				newChessboard[row][col].field_ID = chessboard[row][col].field_ID;
				newChessboard[row][col].figure = move.figure;
			}
			else
			{
				newChessboard[row][col] = chessboard[row][col];
			}
		}
	}

	return newChessboard;
}

void AI::deleteChessboard(AI_Field** chessboard) 
{
	for (int i = 0; i < 8; ++i)
	{
		delete[] chessboard[i];
	}

	delete[] chessboard;
}
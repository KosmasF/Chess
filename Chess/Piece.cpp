#include "Piece.h"
#include <iostream>

const char* Piece::GetName()
{
	return name;
}

void Piece::Draw(int idx, Board* board, Sprites* sprites)
{
	Position pos = Get2DCordsDrawable(idx, board->numSquares , board->SquareSize);
	DrawRectangle(pos.x, pos.y, board->SquareSize, board->SquareSize, DrawColor);
}

Position Piece::Get2DCordsDrawable(int idx , int numSquares , int SquareSize)
{
	int x, y;
	x = idx % numSquares;
	y = idx / numSquares;

	return {x* SquareSize, y* SquareSize };
}

Position Piece::Get2DCords(int idx, int numSquares)
{
	int x, y;
	x = idx % numSquares;
	y = idx / numSquares;

	return { x, y};
}

bool Piece::IsLegal(Piece** pieces, int Original , int ToCheck, Board* board , bool oppositePieceIgnorance , bool ignoreColor)
{
	return false;
}

void Piece::DrawLegal(Piece** pieces, int idx , Board* board)
{
	DrawColor = { 255,0,0,100 };
	for (int i = 0; i < board->totalNumSquares; i++)
	{
		if (IsLegal(pieces,idx, i , board))
		{
			Piece::Draw(i, board, nullptr);
		}
	}
	DrawColor = { 0,255,0,100 };
	Piece::Draw(idx, board, nullptr);
	DrawColor = RED;
}

bool Piece::IsWhite()
{
	return false;
}

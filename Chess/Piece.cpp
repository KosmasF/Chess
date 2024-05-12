#include "Piece.h"
#include <iostream>

const char* Piece::GetName()
{
	return name;
}

void Piece::Draw(int idx, Board* board, Sprites* sprites)
{
	Position pos = Get2DCordsDrawable(idx, board);
	DrawRectangle(pos.x, pos.y, board->SquareSize, board->SquareSize, DrawColor);
}

Position Piece::Get2DCordsDrawable(int idx , Board* board)
{
	int x, y;
	x = idx % board->numSquares;
	y = idx / board->numSquares;

	return {x* board->SquareSize + board->offsetX, y* board->SquareSize + board->offsetY};
}

Position Piece::Get2DCords(int idx, int numSquares)
{
	int x, y;
	x = idx % numSquares;
	y = idx / numSquares;

	return { x, y};
}

bool Piece::IsLegal(Piece** pieces, int Original , int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance , bool ignoreColor)
{
	return false;
}

void Piece::DrawLegal(Piece** pieces, int idx , Board* board, bool* allowCastling , bool inversed)
{
	DrawColor = { 255,0,0,100 };
	for (int i = 0; i < board->totalNumSquares; i++)
	{
		if (IsLegal(pieces,idx, i , board, allowCastling))
		{
			Piece::Draw(inversed ? InverseIndex(i) : i, board, nullptr);
		}
	}
	DrawColor = { 0,255,0,100 };
	Piece::Draw(inversed ? InverseIndex(idx) : idx, board, nullptr);
	DrawColor = RED;
}

bool Piece::IsWhite()
{
	return false;
}

int Piece::InverseIndex(int idx)
{
	Position cords = Get2DCords(idx , 8);

	cords.y = 7 - cords.y;

	return cords.y * 8 + cords.x;
}

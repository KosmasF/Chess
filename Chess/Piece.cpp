#include "Piece.h"
#include <iostream>
#include "PieceTypes.h"

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

bool Piece::IsLegal(Piece** pieces, int Original , int ToCheck, Board* board, bool* allowCastling, bool oppositePieceIgnorance , bool ignoreColor, bool CheckKing)
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

bool Piece::IsKingInAttack(Piece** pieces, bool CheckKing , int Original , int ToCheck , Board* board, bool* allowCastling)
{
	if (!CheckKing)
		return true;

	King* king = nullptr;
	int kingIndex = 64;
	for (int i = 0; i < 64; i++)
	{
		if (pieces[i] != nullptr)
			if (pieces[i]->GetName() == "K" && pieces[i]->IsWhite() == IsWhite())
			{
				king = (King*)pieces[i];
				kingIndex = i;
			}
	}
	if (king == nullptr)
		return false;

	Piece* original = pieces[Original];
	Piece* destination = pieces[ToCheck];

	pieces[ToCheck] = original;
	pieces[Original] = nullptr;

	if (king->IsAttacked(pieces, kingIndex, board, allowCastling))
	{
		pieces[Original] = original;
		pieces[ToCheck] = destination;
		return false;
	}

	pieces[Original] = original;
	pieces[ToCheck] = destination;
	return true;
}

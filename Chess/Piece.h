#pragma once
#include "raylib.h"
#include "Board.h"
#include "Sprites.h"

struct Position {
	int x;
	int y;
};

class King;

class Piece
{
public:
	const char* name = "Invalid!";

	virtual const char* GetName();

	Color DrawColor = RED;

	virtual void Draw(int idx, Board* board, Sprites* sprites);

	Position Get2DCordsDrawable(int idx, Board* board);

	static Position Get2DCords(int idx, int numSquares);

	virtual bool IsLegal(Piece** pieces, int Original, int ToCheck, Board* board , bool* allowCastling, bool oppositePieceIgnorance = true, bool ignoreColor = false, bool CheckKing = true);

	void DrawLegal(Piece** pieces, int idx, Board* board, bool* allowCastling, bool inversed = false);

	virtual bool IsWhite();

	int InverseIndex(int idx);

	bool IsKingInAttack(Piece** pieces, bool CheckKing, int Original, int ToCheck, Board* board, bool* allowCastling);

	bool EnPassantOrNullptr(Piece* piece , bool pawnEating = false);
};


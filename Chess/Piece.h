#pragma once
#include "raylib.h"
#include "Board.h"
#include "Sprites.h"

struct Position {
	int x;
	int y;
};

class Piece
{
public:
	const char* name = "Invalid!";

	virtual const char* GetName();

	Color DrawColor = RED;

	virtual void Draw(int idx, Board* board, Sprites* sprites);

	Position Get2DCordsDrawable(int idx, int numSquares, int SquareSize);

	Position Get2DCords(int idx, int numSquares);

	virtual bool IsLegal(Piece** pieces, int Original, int ToCheck, Board* board , bool oppositePieceIgnorance = true, bool ignoreColor = false);

	void DrawLegal(Piece** pieces, int idx , Board* board);

	virtual bool IsWhite();
};

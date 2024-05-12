#pragma once
#include "raylib.h"
#include <iostream>
#include "InversedPieces.h"

class Board
{
public:
	int width, height;
	int offsetX, offsetY;
	static const int numSquares = 8;
	static const int totalNumSquares = numSquares * numSquares;
	const Color lightColor = {240, 217, 181,255 };
	const Color darkColor = { 181, 136, 99,255 };
	int SquareSize;

	int CollectedPiece = -1;

	void* WhiteEnPassant;
	void* BlackEnPassant;

	bool Inversed;

	Board(int size , int xOFFSET, int yOFFSET , bool inversed);

	~Board();

	void CheckInput(PiecesArray pieces , void* WhiteDefaultPromotionPiece,void* BlackDefaultPromotionPiece, bool* allowCastling);
	void Draw();
	char* MovementNotation(PiecesArray pieces , int Destination , int Location,bool* allowCastling);
	Vector2 Get2DCords(Vector2 pos);
};
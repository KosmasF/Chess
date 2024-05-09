#pragma once
#include "raylib.h"
#include <iostream>

class Board
{
public:
	int width, height;
	static const int numSquares = 8;
	static const int totalNumSquares = numSquares * numSquares;
	const Color lightColor = {240, 217, 181,255 };
	const Color darkColor = { 181, 136, 99,255 };
	int SquareSize;

	int CollectedPiece = -1;

	Board(int size);

	void CheckInput(void** Ipieces , void* WhiteDefaultPromotionPiece,void* BlackDefaultPromotionPiece);
	void Draw();
	char* MovementNotation(void** Ipieces , int Destination , int Location);
	Vector2 Get2DCords(Vector2 pos);
};
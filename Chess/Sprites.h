#pragma once
#include "raylib.h"
#include "Board.h"

class Sprites
{
private:
	Texture2D GetAndResize(Image loadedImage, Rectangle crop, Board* board);
public:
	Sprites(const char* spritesheetPath, Board* board);
	Texture2D WhiteKing, WhiteQueen, WhiteBishop, WhiteKnight, WhiteRook, WhitePawn;
	Texture2D BlackKing, BlackQueen, BlackBishop, BlackKnight, BlackRook, BlackPawn;
};


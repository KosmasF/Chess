#include "Sprites.h"

Texture2D Sprites::GetAndResize(Image* loadedImage ,Rectangle crop , Board* board)
{
	Image img = ImageFromImage(*loadedImage, crop);
	ImageResize(&img, board->SquareSize, board->SquareSize);
	return LoadTextureFromImage(img);
}

Sprites::Sprites(const char* spritesheetPath, Board* board)// Default board is okay
{
	Image image = LoadImage(spritesheetPath);
	float width = image.width / 6;//Num of horizontal pieces
	float height = image.height / 2;// Vertical pieces


	WhiteKing = GetAndResize(&image, {0,0,width,height} , board);
	WhiteQueen = GetAndResize(&image, { width,0,width,height }, board);
	WhiteBishop = GetAndResize(&image, { width*2,0,width,height }, board);
	WhiteKnight = GetAndResize(&image, { width*3,0,width,height }, board);
	WhiteRook = GetAndResize(&image, { width*4,0,width,height }, board);
	WhitePawn = GetAndResize(&image, { width*5,0,width,height }, board);


	BlackKing= GetAndResize(&image, { 0,height,width,height }, board);
	BlackQueen = GetAndResize(&image, { width,height,width,height }, board);
	BlackBishop = GetAndResize(&image, { width*2,height,width,height }, board);
	BlackKnight = GetAndResize(&image, { width*3,height,width,height }, board);
	BlackRook = GetAndResize(&image, { width*4,height,width,height }, board);
	BlackPawn = GetAndResize(&image, { width*5,height,width,height }, board);
}
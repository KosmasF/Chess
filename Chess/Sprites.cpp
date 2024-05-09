#include "Sprites.h"

Texture2D Sprites::GetAndResize(Image loadedImage ,Rectangle crop , Board* board)
{
	ImageCrop(&loadedImage, crop);
	ImageResize(&loadedImage, board->SquareSize, board->SquareSize);
	return LoadTextureFromImage(loadedImage);
}

Sprites::Sprites(const char* spritesheetPath, Board* board)
{
	Image image = LoadImage(spritesheetPath);
	float width = image.width / 6;//Num of horizontal pieces
	float height = image.height / 2;// Vertical pieces


	WhiteKing = GetAndResize(LoadImage(spritesheetPath), {0,0,width,height} , board);
	WhiteQueen = GetAndResize(LoadImage(spritesheetPath), { width,0,width,height }, board);
	WhiteBishop = GetAndResize(LoadImage(spritesheetPath), { width*2,0,width,height }, board);
	WhiteKnight = GetAndResize(LoadImage(spritesheetPath), { width*3,0,width,height }, board);
	WhiteRook = GetAndResize(LoadImage(spritesheetPath), { width*4,0,width,height }, board);
	WhitePawn = GetAndResize(LoadImage(spritesheetPath), { width*5,0,width,height }, board);


	BlackKing= GetAndResize(LoadImage(spritesheetPath), { 0,height,width,height }, board);
	BlackQueen = GetAndResize(LoadImage(spritesheetPath), { width,height,width,height }, board);
	BlackBishop = GetAndResize(LoadImage(spritesheetPath), { width*2,height,width,height }, board);
	BlackKnight = GetAndResize(LoadImage(spritesheetPath), { width*3,height,width,height }, board);
	BlackRook = GetAndResize(LoadImage(spritesheetPath), { width*4,height,width,height }, board);
	BlackPawn = GetAndResize(LoadImage(spritesheetPath), { width*5,height,width,height }, board);
}
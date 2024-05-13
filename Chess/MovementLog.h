#pragma once
#include "raylib.h"
#include <memory>

class MovementLog
{
private:
	const char* header = "MOVEMENT LOG";
	int fontSize = 20;
	int LogSize;
	int ScreenHeight;
	const Color lightBlack = { 58, 57, 54, 255 };
	const char one = '0';
	int LogCenterPos;
	int fistNumToDraw = 0;

	static constexpr int maxMoves = 100;
	int lastMoveIndex = 0;

	const char* moves[maxMoves];

	void DrawHeader();
	void DrawBackground();
public:
	MovementLog(int logSize , int screenHeight);

	void Draw();

	~MovementLog();

	void Check();

	void AddMove(const char* move);

};


#pragma once

#include "raylib.h"
#include "Board.h"
#include <iostream>
#include "Piece.h"
#include "PieceTypes.h"
#include "InversedPieces.h"
#include "Sprites.h"
#include "MovementLog.h"
#include "SocketConnection.h"

//I HATE RAYLIB!!!
//I HATE RAYLIB!!!
//I HATE RAYLIB!!!
//I HATE RAYLIB!!!
//I HATE RAYLIB!!!

template<int size>
struct BranchEvaluationData
{
    int bestMoves[size][2];
    float evals[size];
};


class Game
{
private:
    Board* board;
    Board* BLACK_board;
    Board* boards[2];

    MovementLog* movementLog;

    Sprites* sprites;

    Pawn* WhitePawn;
    Pawn* BlackPawn;
    Bishop* WhiteBishop;
    Bishop* BlackBishop;
    Knight* WhiteKnight;
    Knight* BlackKnight;
    Rook* WhiteRook;
    Rook* BlackRook;
    Queen* WhiteQueen;
    Queen* BlackQueen;
    King* WhiteKing;
    King* BlackKing;

    Piece* WhiteDefaultPromotionPiece;
    Piece* BlackDefaultPromotionPiece;

    bool allowCastling[4];

    Piece* pieces[64];
    PiecesArray Pieces = PiecesArray(pieces, 64);

    SocketConnection stockfish;

    int evalLineHeight = 20;

    bool blackIsPlayer = false;

    static const int defaultBranchSize = 5;

    BranchEvaluationData<defaultBranchSize> BranchEval(const char* position);

    BranchEvaluationData<defaultBranchSize> dataToDraw = BranchEvaluationData<defaultBranchSize>();
public:
	const int screenHeight = 536;
	const int LogSize = 200;
	const int screenWidth = screenHeight * 2 + LogSize;

	const char* spritesheetPath = "spritesheet.png";

	Game(int argc, char** argv);

	~Game();

    static const char* GetFen(PiecesArray FenPieces , bool* castling, int lastMoveIndex);

	void Update();

    void SetPiecesAsDefault(Piece** pieces);
};


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
#include "NeuralNetwork.h"
#include "NonGraphicalBoard.h"

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

    const bool blackIsPlayer = true;

    static const int defaultBranchSize = 5;

    BranchEvaluationData<defaultBranchSize> BranchEval(const char* position);

    BranchEvaluationData<defaultBranchSize> dataToDraw = BranchEvaluationData<defaultBranchSize>();

    const char* path = "networks/testEvaluatorNonRandomWeights.nn";
    NeuralNetwork evaluator = NeuralNetwork(path);

    void DrawBar(float num, int offset);
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

    void Randomize(int seed)
    {
        srand(seed);
        goto Start;
    SetSeed:
        srand(rand());
    Start:
        Piece* AllPieces[] = {
            WhitePawn,
            BlackPawn,
            WhiteBishop,
            BlackBishop,
            WhiteKnight,
            BlackKnight,
            WhiteRook,
            BlackRook,
            WhiteQueen,
            BlackQueen
            //WhiteKing
            //BlackKing
        };
        for (int i = 0; i < 64; i++)
        {
            bool empty = (rand() % 5);
            if (!empty)
            {
                Piece* piece = AllPieces[rand() % (sizeof(AllPieces) / sizeof(Piece*))];
                pieces[i] = piece;
            }
            else
            {
                pieces[i] = nullptr;
            }
        }

    KingSet:

        printf("Calculating..\n");

        int WhiteKingPos = rand() % 64;
        int BlackKingPos = rand() % 64;
        if (WhiteKingPos != BlackKingPos)
        {
            pieces[WhiteKingPos] = WhiteKing;
            pieces[BlackKingPos] = BlackKing;
        }
        else
            goto KingSet;


        if (!(BlackKing->IsAttacked(pieces, BlackKingPos, nullptr, allowCastling)))
        {
            goto End;
        }
        else if (WhiteKing->IsAttacked(pieces, WhiteKingPos, nullptr, allowCastling))
        {
            goto SetSeed;
        }
        else
        {
            movementLog->lastMoveIndex = 1;
        }

    End:

        allowCastling[0] = 0;
        allowCastling[1] = 0;
        allowCastling[2] = 0;
        allowCastling[3] = 0;

        const char* fen = GetFen(Pieces, allowCastling, movementLog->lastMoveIndex);
        printf("Eval %f\n", stockfish.getEval(fen));

        printf("END\n");
        return;
    }
};


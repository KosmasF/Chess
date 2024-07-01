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
#include "fstream"
#include "String"

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
    BranchEvaluationData()
    {
        for (int i = 0; i < size; i++)
        {
            bestMoves[i][0] = bestMoves[i][1] = -1;
            evals[i] = 0;
        }
    }
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

    const bool blackIsPlayer = false;
    const bool showBestMoves = false;

    static const int defaultBranchSize = 5;

    BranchEvaluationData<defaultBranchSize> BranchEval(const char* position);

    BranchEvaluationData<defaultBranchSize> dataToDraw = BranchEvaluationData<defaultBranchSize>();

    const char* path = "networks/testedNonRandom3LayersBIG.nn";
    NeuralNetwork evaluator = NeuralNetwork(path);   
    const char* pathNEW = "networks/nnRe-evalInMasterGames.nn";
    NeuralNetwork evaluatorNEW = NeuralNetwork(pathNEW);
    const char* path2 = "networks/nnRe-evalInMasterGamesErrorCorrection.nn";
    NeuralNetwork evaluator2 = NeuralNetwork(path2);

    void DrawBar(float num, int offset);

    std::fstream* gameFile;
    const char* gameFilePath = "games/output.pgn";
    std::fstream* OpenGameFile(const char* path, int gameIndex);

    const char* predictorPath = "networks/predictorTest.nn";
    NeuralNetwork predictor = NeuralNetwork(predictorPath);

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

    void Randomize(int seed);

};


#pragma once

#include "raylib.h"
#include "Board.h"
#include <iostream>
#include "Piece.h"
#include "PieceTypes.h"
#include "InversedPieces.h"
#include "Sprites.h"
#include "MovementLog.h"
#include "NeuralNetwork.h"
#include "NonGraphicalBoard.h"
#include "fstream"
#include "string"
#include "GPU.h"


//#define USE_STOCKFISH_SERVER

#ifdef USE_STOCKFISH_SERVER

#include "SocketConnection.h"

#else

class Stockfish//TEMPORARY
{
    public:
    float getEval(const char* fen) {return 0;}
};

#endif


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
    BranchEvaluationData(int lastMoveIndex = 1)
    {
        for (int i = 0; i < size; i++)
        {
            bestMoves[i][0] = bestMoves[i][1] = lastMoveIndex % 2 == 0 ? -153 : 153;
            evals[i] = 0;
        }
    }
};

extern GPU gpu;

enum EvalutorType
{
    STOCKFISH,
    NEURALNETWORK
};

struct EvalutionType
{
    EvalutorType type;
    void* pos;
};

template<int size>
struct BranchOutputEvaluation
{
    int branchID[size];
    int moves[size][2];
};

template<int depth>
struct EvaluationData
{
    int moves[depth][2];
    float evaluation;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//  In order to make static destructor, we need a instance counter  // 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

inline int gameInstances;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


class Game
{
private:
    Board* board;
    Board* BLACK_board;
    Board* boards[2];

    MovementLog* movementLog;

    Sprites* sprites;

    static Pawn* WhitePawn;
    static Pawn* BlackPawn;
    static Bishop* WhiteBishop;
    static Bishop* BlackBishop;
    static Knight* WhiteKnight;
    static Knight* BlackKnight;
    static Rook* WhiteRook;
    static Rook* BlackRook;
    static Queen* WhiteQueen;
    static Queen* BlackQueen;
    static King* WhiteKing;
    static King* BlackKing;

    static Piece* WhiteDefaultPromotionPiece;
    static Piece* BlackDefaultPromotionPiece;

    bool allowCastling[4];

    Piece* pieces[64];
    PiecesArray Pieces = PiecesArray(pieces, 64);

    #ifdef USE_STOCKFISH_SERVER
        SocketConnection stockfish;
    #else
        Stockfish stockfish;
    #endif

    int evalLineHeight = 20;

    const bool blackIsPlayer = false;
    const bool showBestMoves = false;

    static const int defaultBranchSize = 5;

    static BranchEvaluationData<defaultBranchSize> BranchEval(EvalutionType evaluator, Piece** pieces, bool* allowCastling, int lastMoveIndex);
    static float GetPosEval(EvalutionType evaluator, Piece** pieces, bool* allowCastling, int lastMoveIndex);

    template <uint depth>
    static EvaluationData<depth> Eval(EvalutionType evaluator, const Piece** const pieces, const bool* const allowCastling, int lastMoveIndex);

    BranchEvaluationData<defaultBranchSize> dataToDraw = BranchEvaluationData<defaultBranchSize>();

    const char* path = "../networks/testedNonRandom3LayersBIG.nn";
    NeuralNetwork evaluator = NeuralNetwork(path, &gpu);   
    const char* pathNEW = "../networks/nnRe-evalInMasterGames.nn";
    NeuralNetwork evaluatorNEW = NeuralNetwork(pathNEW, &gpu);
    const char* path2 = "../networks/nnRe-evalInMasterGamesErrorCorrection.nn";
    NeuralNetwork evaluator2 = NeuralNetwork(path2, &gpu);

    void DrawBar(float num, int offset);

    std::fstream* gameFile;
    const char* gameFilePath = "../games/output.pgn";
    std::fstream* OpenGameFile(const char* path, int gameIndex);

    const char* predictorPath = "../networks/predictorTest.nn";
    NeuralNetwork predictor = NeuralNetwork(predictorPath, &gpu);

public:
	const int screenHeight = 536;
	const int LogSize = 200;
	const int screenWidth = screenHeight * 2 + LogSize;

	const char* spritesheetPath = "../spritesheet.png";

	Game(int argc, char** argv);

	~Game();

    static const char* GetFen(PiecesArray FenPieces , bool* castling, int lastMoveIndex);

	void Update();

    void SetPiecesAsDefault(Piece** pieces);

    void Randomize(int seed);

};


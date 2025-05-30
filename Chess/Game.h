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
    void print(Piece** pieces, bool* allowCastling)
    {
        for(int i = 0; i < size; i++)
        {
            printf("| Move: %s ,Eval: %f |", Board::MovementNotation(PiecesArray(pieces, Board::totalNumSquares), bestMoves[i][1], bestMoves[i][0], allowCastling), evals[i]);
        }
        printf("\n");
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

    void print(Piece** pieces, bool* allowCastling)
    {
        for(int i = 0; i < size; i++)
        {
            printf("| Move: %s ,Branch: %d |", Board::MovementNotation(PiecesArray(pieces, Board::totalNumSquares), moves[i][1], moves[i][0], allowCastling), branchID[i]);
        }
        printf("\n");
    }
    void print(Piece** pieces, bool* allowCastling, int move)
    {
        printf("| Move: %s ,Branch: %d |", Board::MovementNotation(PiecesArray(pieces, Board::totalNumSquares), moves[move][1], moves[move][0], allowCastling), branchID[move]);
    } 
};


//Rarely needed
template<int depth>
struct EvaluationData
{
    int moves[depth][2];
    float evaluation;
};

struct Move
{
    int From;
    int To;
    int& operator[](int i)
    {
        if(i == 0)
            return From;
        else if(i == 1)
            return To;
    }
};

struct HeapMovementData
{
    Move* moves;
    HeapMovementData(uint depth)
    {
        moves = (Move *)malloc(sizeof(Move) * depth);
        branch = (int *)malloc(sizeof(int) * depth);
        move_depth = depth;
    }
    ~HeapMovementData()
    {
        free(moves);
    }
    HeapMovementData(HeapMovementData& other)//copy contuctor
    {
        move_depth = other.move_depth;
        branch = other.branch;
        moves = (Move*)malloc(sizeof(Move*) * move_depth);
        memcpy(moves, other.moves, sizeof(Move*) * move_depth);
        branch = (int *)malloc(sizeof(int) * move_depth);
        memcpy(branch, other.branch, sizeof(int) * move_depth);
    }
    uint move_depth;
    int* branch;
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

    static BranchEvaluationData<defaultBranchSize> BranchEval(EvalutionType evaluator, Piece** pieces, bool* allowCastling, int lastMoveIndex, bool bestMove = true);
    static float GetPosEval(EvalutionType evaluator, Piece** pieces, bool* allowCastling, int lastMoveIndex);

    template <uint depth>
    static EvaluationData<depth> OptimalEval(EvalutionType evaluator, const Piece** const pieces, const bool* const allowCastling, int lastMoveIndex);

    static HeapMovementData GetTreeMoves(BranchEvaluationData<defaultBranchSize> base, BranchOutputEvaluation<defaultBranchSize>* phases, int idx, uint depth);

    static HeapMovementData Eval(EvalutionType evaluator, Piece** pieces, bool* allowCastling, int lastMoveIndex, uint depth);

    static BranchOutputEvaluation<defaultBranchSize> GetBestMoves(BranchEvaluationData<defaultBranchSize * defaultBranchSize> moves, int lastMoveIndex ,bool best = true);
    static float* GetBestMoveEvals(BranchEvaluationData<defaultBranchSize * defaultBranchSize> moves, int lastMoveIndex ,bool best = true);

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


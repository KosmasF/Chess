#pragma once
#include "Board.h"
#include "Piece.h"
#include "PieceTypes.h"

class NonGraphicalBoard
{
public:
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

    bool whitePlays = true;

	NonGraphicalBoard();

    ~NonGraphicalBoard();

    float* Status(bool isWhite);

    static float* Status(bool isWhite, Piece** pieces,
        Pawn* WhitePawn,
        Pawn* BlackPawn,
        Bishop* WhiteBishop,
        Bishop* BlackBishop,
        Knight* WhiteKnight,
        Knight* BlackKnight,
        Rook* WhiteRook,
        Rook* BlackRook,
        Queen* WhiteQueen,
        Queen* BlackQueen,
        King* WhiteKing,
        King* BlackKing
    );

    void SetPiecesAsDefault(Piece** pieces);

    bool Move(int From, int To , bool IsWhite);

    void PrintStatus(bool isWhite);

    void Randomize(int seed, bool allowBlackToMove = true);
};


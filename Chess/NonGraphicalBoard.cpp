#include "NonGraphicalBoard.h"

NonGraphicalBoard::NonGraphicalBoard()
{
    WhitePawn = new Pawn(WHITE);
    BlackPawn = new Pawn(BLACK);
    WhiteBishop = new Bishop(WHITE);
    BlackBishop = new Bishop(BLACK);
    WhiteKnight = new Knight(WHITE);
    BlackKnight = new Knight(BLACK);
    WhiteRook = new Rook(WHITE);
    BlackRook = new Rook(BLACK);
    WhiteQueen = new Queen(WHITE);
    BlackQueen = new Queen(BLACK);
    WhiteKing = new King(WHITE);
    BlackKing = new King(BLACK);


    WhiteDefaultPromotionPiece = WhiteQueen;
    BlackDefaultPromotionPiece = BlackQueen;

    allowCastling[0] = true;
    allowCastling[1] = true;
    allowCastling[2] = true;
    allowCastling[3] = true;

    SetPiecesAsDefault(pieces);
}

NonGraphicalBoard::~NonGraphicalBoard()
{
    delete WhitePawn;
    delete BlackPawn;
    delete WhiteBishop;
    delete BlackBishop;
    delete WhiteKnight;
    delete BlackKnight;
    delete WhiteRook;
    delete BlackRook;
    delete WhiteQueen;
    delete BlackQueen;
    delete WhiteKing;
    delete BlackKing;
}

float* NonGraphicalBoard::Status(bool isWhite)
{
    float* status = new float[64];

    for (int i = 0; i < 64; i++)
    {
        if (pieces[i] == nullptr)
        {
            status[i] = 0;
            continue;
        }
        if (pieces[i] == WhitePawn)
        {
            status[i] = 1;
            continue;
        }
        if (pieces[i] == BlackPawn)
        {
            status[i] = -1;
            continue;
        }
        if (pieces[i] == WhiteKnight)
        {
            status[i] = 2;
            continue;
        }
        if (pieces[i] == BlackKnight)
        {
            status[i] = -2;
            continue;
        }
        if (pieces[i] == WhiteBishop)
        {
            status[i] = 3;
            continue;
        }
        if (pieces[i] == BlackBishop)
        {
            status[i] = -3;
            continue;
        }
        if (pieces[i] == WhiteRook)
        {
            status[i] = 5;
            continue;
        }
        if (pieces[i] == BlackRook)
        {
            status[i] = -5;
            continue;
        }
        if (pieces[i] == WhiteQueen)
        {
            status[i] = 9;
            continue;
        }
        if (pieces[i] == BlackQueen)
        {
            status[i] = -9;
            continue;
        }
        if (pieces[i] == WhiteKing)
        {
            status[i] = 10;
            continue;
        }
        if (pieces[i] == BlackKing)
        {
            status[i] = -10;
            continue;
        }

        status[i] = 0;

    }

    if (!isWhite)
        for (int i = 0; i < 64; i++)
            status[i] *= -1;

    for (int i = 0; i < 64; i++)
        status[i] /= -10;

    return status;
}

void NonGraphicalBoard::SetPiecesAsDefault(Piece** pieces)
{
    for (int i = 0; i < 64; i++)
    {
        pieces[i] = nullptr;
    }

    pieces[0] = BlackRook;
    pieces[1] = BlackKnight;
    pieces[2] = BlackBishop;
    pieces[3] = BlackQueen;
    pieces[4] = BlackKing;
    pieces[5] = BlackBishop;
    pieces[6] = BlackKnight;
    pieces[7] = BlackRook;
    pieces[8] = BlackPawn;
    pieces[9] = BlackPawn;
    pieces[10] = BlackPawn;
    pieces[11] = BlackPawn;
    pieces[12] = BlackPawn;
    pieces[13] = BlackPawn;
    pieces[14] = BlackPawn;
    pieces[15] = BlackPawn;

    pieces[48] = WhitePawn;
    pieces[49] = WhitePawn;
    pieces[50] = WhitePawn;
    pieces[51] = WhitePawn;
    pieces[52] = WhitePawn;
    pieces[53] = WhitePawn;
    pieces[54] = WhitePawn;
    pieces[55] = WhitePawn;
    pieces[56] = WhiteRook;
    pieces[57] = WhiteKnight;
    pieces[58] = WhiteBishop;
    pieces[59] = WhiteQueen;
    pieces[60] = WhiteKing;
    pieces[61] = WhiteBishop;
    pieces[62] = WhiteKnight;
    pieces[63] = WhiteRook;
}

bool NonGraphicalBoard::Move(int From, int To, bool IsWhite)
{
    if (!IsWhite)
    {
        From = WhiteDefaultPromotionPiece->InverseIndex(From);
        To = WhiteDefaultPromotionPiece->InverseIndex(To);
    }

    if (pieces[From] != nullptr)
    {
        if (pieces[From]->IsWhite() != IsWhite)
            return false;
        if (pieces[From]->IsLegal(pieces, From, To, nullptr, allowCastling))
        {
            pieces[To] = pieces[From];
            pieces[From] = nullptr;
            return true;
        }
    }

    return false;
}

void NonGraphicalBoard::PrintStatus(bool isWhite)
{
    float* status = Status(isWhite);

    for (int i = 0; i < 64; i++)
    {
        std::cout << status[i] << "    ";
        if (i % 8 == 7)
            std::cout << '\n';
    }
    std::cout << std::endl;

    delete[] status;
}

void NonGraphicalBoard::Randomize(int seed, bool allowBlackToMove)
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

    whitePlays = true;

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

    //printf("Calculating..\n");

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
        if (allowBlackToMove)
            whitePlays = false;
        else
            goto SetSeed;
    }

End:

    allowCastling[0] = 0;
    allowCastling[1] = 0;
    allowCastling[2] = 0;
    allowCastling[3] = 0;

    //const char* fen = GetFen(Pieces, allowCastling, movementLog->lastMoveIndex);
    //printf("Eval %f\n", stockfish.getEval(fen));

    //printf("END\n");
    return;
}

float* NonGraphicalBoard::Status(bool isWhite , Piece** pieces, 
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
)
{
    float* status = new float[64];

    for (int i = 0; i < 64; i++)
    {
        if (pieces[i] == nullptr)
        {
            status[i] = 0;
            continue;
        }
        if (pieces[i] == WhitePawn)
        {
            status[i] = 1;
            continue;
        }
        if (pieces[i] == BlackPawn)
        {
            status[i] = -1;
            continue;
        }
        if (pieces[i] == WhiteKnight)
        {
            status[i] = 2;
            continue;
        }
        if (pieces[i] == BlackKnight)
        {
            status[i] = -2;
            continue;
        }
        if (pieces[i] == WhiteBishop)
        {
            status[i] = 3;
            continue;
        }
        if (pieces[i] == BlackBishop)
        {
            status[i] = -3;
            continue;
        }
        if (pieces[i] == WhiteRook)
        {
            status[i] = 5;
            continue;
        }
        if (pieces[i] == BlackRook)
        {
            status[i] = -5;
            continue;
        }
        if (pieces[i] == WhiteQueen)
        {
            status[i] = 9;
            continue;
        }
        if (pieces[i] == BlackQueen)
        {
            status[i] = -9;
            continue;
        }
        if (pieces[i] == WhiteKing)
        {
            status[i] = 10;
            continue;
        }
        if (pieces[i] == BlackKing)
        {
            status[i] = -10;
            continue;
        }

        status[i] = 0;

    }

    if (!isWhite)
        for (int i = 0; i < 64; i++)
            status[i] *= -1;

    for (int i = 0; i < 64; i++)
        status[i] /= -10;

    return status;
}
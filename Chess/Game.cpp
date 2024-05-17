#include "Game.h"

Game::Game()
{
    InitWindow(screenWidth, screenHeight, "Chess");
    SetTargetFPS(60);

    board = new Board(screenHeight, 0, 0, false);
    BLACK_board = new Board(screenHeight, screenHeight + LogSize, 0, true);
    boards[0] = board;
    boards[1] = BLACK_board;


    if (screenHeight % 8 != 0)
    {
        throw "Screen height is not divisible by 8";
    }

    movementLog = new MovementLog(LogSize, screenHeight);

    sprites = new Sprites(spritesheetPath, board);

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

Game::~Game()
{
    delete board;
    delete BLACK_board;
    delete movementLog;
    delete sprites;
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
    CloseWindow();
}

void Game::Update()
{
    for (int i =0 ; i < sizeof boards / sizeof(Board*) ; i ++)
    {
        Board* board = boards[i];

        board->Draw();

        if (!board->Inversed)
        {
            if (!(movementLog->lastMoveIndex % 2))
                board->CheckInput(Pieces, (void*)WhiteDefaultPromotionPiece, (void*)BlackDefaultPromotionPiece, allowCastling, movementLog);

            for (int i = 0; i < board->totalNumSquares; i++)
            {
                if (Pieces[i] != nullptr)
                {
                    Pieces[i]->Draw(i, board, sprites);
                }
            }

            if (board->CollectedPiece != -1 && Pieces[board->CollectedPiece] != nullptr)
                Pieces[board->CollectedPiece]->DrawLegal(Pieces, board->CollectedPiece, board, allowCastling);

        }
        else
        {
            if ((movementLog->lastMoveIndex % 2))
                board->CheckInput(Pieces, (void*)WhiteDefaultPromotionPiece, (void*)BlackDefaultPromotionPiece, allowCastling, movementLog);

            Position idx = { 7,0 };
            for (int i = board->totalNumSquares - 1; i > -1; i--)
            {
                if (Pieces[i] != nullptr)
                {
                    Pieces[i]->Draw(idx.y * board->numSquares + idx.x, board, sprites);
                }
                idx.x--;
                if (idx.x == -1)
                {
                    idx.x = 7;
                    idx.y++;
                }
            }

            if (board->CollectedPiece != -1 && Pieces[board->CollectedPiece] != nullptr)
                Pieces[board->CollectedPiece]->DrawLegal(Pieces, board->CollectedPiece, board, allowCastling, true);
        }
    }


    if (IsKeyPressed(KEY_R))
    {
        SetPiecesAsDefault(pieces);
    }

    movementLog->Draw();


    //DrawFPS(0, 0);
    EndDrawing();
}

void Game::SetPiecesAsDefault(Piece** pieces)
{
    for (int i = 0; i < board->totalNumSquares; i++)
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

    movementLog->lastMoveIndex = 0;
}

#include "raylib.h"
#include "Board.h"
#include <iostream>
#include "Piece.h"
#include "PieceTypes.h"
#include "Sprites.h"

#define print(x) std::cout<<x<<std::endl;


int main() {

    const int screenWidth = 1920 / 2;
    const int screenHeight = 536;

    const char* spritesheetPath = "spritesheet.png";


    InitWindow(screenWidth, screenHeight, "Chess");
    SetTargetFPS(60);

    Board* board = new Board(screenHeight);   
    if (screenHeight % 8 != 0)
    {
        throw "Screen height is not divisible by 8";
    }
    Sprites* sprites = new Sprites(spritesheetPath,board);

    Pawn* WhitePawn = new Pawn(WHITE);
    Pawn* BlackPawn = new Pawn(BLACK);
    Bishop* WhiteBishop = new Bishop(WHITE);
    Bishop* BlackBishop = new Bishop(BLACK);
    Knight* WhiteKnight = new Knight(WHITE);
    Knight* BlackKnight = new Knight(BLACK);
    Rook* WhiteRook = new Rook(WHITE);
    Rook* BlackRook = new Rook(BLACK);
    Queen* WhiteQueen = new Queen(WHITE);
    Queen* BlackQueen = new Queen(BLACK);
    King* WhiteKing = new King(WHITE);
    King* BlackKing = new King(BLACK);


    Piece* WhiteDefaultPromotionPiece = WhiteQueen;
    Piece* BlackDefaultPromotionPiece = BlackQueen;

    Piece* pieces[board->totalNumSquares];
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

    while (!WindowShouldClose()) 
    {

        BeginDrawing();

        ClearBackground(RAYWHITE);
        board->CheckInput((void**)pieces,(void*)WhiteDefaultPromotionPiece,(void*)BlackDefaultPromotionPiece);

        board->Draw();
        for (int i = 0; i < board->totalNumSquares; i++)
        {
            if (pieces[i] != nullptr)
            {
                pieces[i]->Draw(i, board , sprites);
            }
        }

        if (board->CollectedPiece != -1 && pieces[board->CollectedPiece] != nullptr)
            pieces[board->CollectedPiece]->DrawLegal(pieces, board->CollectedPiece, board);


        EndDrawing();
    }

    delete board;
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
    return 0;
}
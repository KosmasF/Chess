#include "Game.h"
#include "ActivationMethods.h"

Game::Game(int argc , char** argv)
{
    InitWindow(screenWidth, screenHeight+evalLineHeight, "Chess");
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

    printf("Connecting with server...\n");
    bool successful = stockfish.Setup(argc,argv);

    if (!successful)
    {
        printf("Connected Successfully with Stockfish server!\n");
    }
    else
    {
        printf("Failed connection with Stockfish!\n");
    }
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

const char* Game::GetFen()
{

    char* value = Pieces.fen();
    int buffer = strlen(value);

    value[buffer] = ' ';
    buffer++;

    bool whiteMoves = !(movementLog->lastMoveIndex % 2);
    if (whiteMoves)
    {
        value[buffer] = 'w';
        buffer++;
    }
    else
    {
        value[buffer] = 'b';
        buffer++;
    }

    value[buffer] = ' ';
    buffer++;

    if (allowCastling[0])
    {
        value[buffer] = 'K';
        buffer++;
    }
    if (allowCastling[1])
    {
        value[buffer] = 'Q';
        buffer++;
    }
    if (allowCastling[2])
    {
        value[buffer] = 'k';
        buffer++;
    }
    if (allowCastling[3])
    {
        value[buffer] = 'q';
        buffer++;
    }

    value[buffer] = ' ';
    buffer++;


    bool foundPawn = false;
    for (int i = 0; i < board->numSquares; i++)
    {
        if (pieces[i] == board->WhiteEnPassant && !whiteMoves)
        {
            Position pos = WhiteDefaultPromotionPiece->Get2DCords(i, board->numSquares);
            value[buffer] = (char)(pos.x + 97);
            buffer++;
            value[buffer] = (char)(pos.y + 49);
            buffer++;
            foundPawn = true;
        }
        if (pieces[i] == board->BlackEnPassant && whiteMoves)
        {
            Position pos = BlackDefaultPromotionPiece->Get2DCords(i, board->numSquares);
            value[buffer] = (char)(pos.x + 97);
            buffer++;
            value[buffer] = (char)(pos.y + 49);
            buffer++;
            foundPawn = true;
        }
    }

    if (!foundPawn)
    {
        value[buffer] = '-';
        buffer++;
    }

    value[buffer] = ' ';
    buffer++;

    value[buffer] = '0';//We haven't made drawing mechanism yet!
    buffer++;

    value[buffer] = ' ';
    buffer++;

    value[buffer] = movementLog->lastMoveIndex + 48;
    buffer++;

    value[buffer] = 0;
    buffer++;

    return value;
}

void Game::Update()
{

    BeginDrawing();
    ClearBackground(RAYWHITE);

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

    const char* fen = GetFen();
    
    float eval = stockfish.getEval(fen);
    float originalEval = eval;


    delete[] fen;

    DrawRectangle(0, screenHeight, screenWidth, evalLineHeight, BLACK);

    eval = FastSigmoid(eval,3);

    float evalWidth = eval*(screenWidth/2);//0 - 153 - > 0 - screenWidth/2
    DrawRectangle(0, screenHeight, screenWidth/2 + evalWidth, evalLineHeight, WHITE);


    if (eval >= 0)
    {
        char buffer[5];

        snprintf(buffer, sizeof(buffer), "%f", originalEval);

        DrawText(buffer, 20, screenHeight, 20, BLACK);
    }
    else
    {
        char buffer[5];

        snprintf(buffer, sizeof(buffer), "%f", originalEval *-1);

        DrawText(buffer, screenWidth - MeasureText(buffer, 20) - 20, screenHeight, 20, WHITE);

    }
    

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

#include "Game.h"
#include "ActivationMethods.h"

Game::Game(int argc , char** argv)
{
    //throw;
    InitWindow(screenWidth, screenHeight +  (evalLineHeight * 4), "Chess");
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

    #ifdef USE_STOCKFISH_SERVER

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

    #endif

    gameFile = OpenGameFile(gameFilePath, 1);

}

Game::~Game()
{
    delete board;
    delete BLACK_board;
    delete Board::WhiteEnPassant;//Static variable
    delete Board::BlackEnPassant;//Static variable
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
    gameFile->close();
    delete gameFile;
}

const char* Game::GetFen(PiecesArray FenPieces, bool* castling, int lastMoveIndex)
{

    char* value = FenPieces.fen();
    int buffer = strlen(value);

    value[buffer] = ' ';
    buffer++;

    bool whiteMoves = !(lastMoveIndex % 2);
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

    if (castling[0])
    {
        value[buffer] = 'K';
        buffer++;
    }
    if (castling[1])
    {
        value[buffer] = 'Q';
        buffer++;
    }
    if (castling[2])
    {
        value[buffer] = 'k';
        buffer++;
    }
    if (castling[3])
    {
        value[buffer] = 'q';
        buffer++;
    }

    value[buffer] = ' ';
    buffer++;


    bool foundPawn = false;
    for (int i = 0; i < 64; i++)
    {
        if (FenPieces[i] != nullptr)
        {
            if (strcmp(FenPieces[i]->GetName() , "Invalid!") == 0 && FenPieces[i]->IsWhite() && !whiteMoves)
            {
                Position pos = Piece::Get2DCords(i, 8);
                value[buffer] = (char)(pos.x + 97);
                buffer++;
                value[buffer] = (char)(pos.y + 49);
                buffer++;
                foundPawn = true;
            }
            if (strcmp(FenPieces[i]->GetName() , "Invalid!") == 0 && !(FenPieces[i]->IsWhite()) && whiteMoves)
            {
                Position pos = Piece::Get2DCords(i, 8);
                value[buffer] = (char)(pos.x + 97);
                buffer++;
                value[buffer] = (char)(pos.y + 49);
                buffer++;
                foundPawn = true;
            }
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

    value[buffer] = lastMoveIndex + 48;
    buffer++;

    value[buffer] = 0;
    buffer++;

    return value;
}

void Game::Update()
{

    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (int i =0 ; i < (int)(sizeof(boards) / sizeof(Board*)); i ++)
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
        {//HERE BLACK PLAYS

            if ((movementLog->lastMoveIndex % 2))
            {
                if (blackIsPlayer)
                {
                    board->CheckInput(Pieces, (void*)WhiteDefaultPromotionPiece, (void*)BlackDefaultPromotionPiece, allowCastling, movementLog);
                }
                else
                {
                    const char* fen = GetFen(Pieces,allowCastling,movementLog->lastMoveIndex);
                    //float currentEval = stockfish.getEval(fen);

                    int maxEvalDiff = -( - 154 - 154);
                    int bestMoveIndex;


                    BranchEvaluationData<defaultBranchSize> data = BranchEval(fen);
                    delete[] fen;

                    //char* movementNotation = board->MovementNotation(Pieces, bestMove[1], bestMove[0], allowCastling);
                    //movementLog->AddMove(movementNotation);
                    //delete[] movementNotation;

                    for (int i = 0; i < defaultBranchSize; i++)
                    {
                        if (data.evals[i] < maxEvalDiff)
                        {
                            bestMoveIndex = i;
                            maxEvalDiff = data.evals[i];
                        }
                        //board->DrawMove(data.bestMoves[i][0], data.bestMoves[i][1]);
                    }
                    dataToDraw = data;
                    
                    board->MakeMove(data.bestMoves[bestMoveIndex][0], data.bestMoves[bestMoveIndex][1], Pieces, allowCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, movementLog, false, board->CollectedPiece, Board::WhiteEnPassant, Board::BlackEnPassant);
                }
            }

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

    if (IsKeyPressed(KEY_G))
    {
       SetPiecesAsDefault(pieces);
       Randomize(time(NULL));
    }
    if (IsKeyPressed(KEY_I))
    {
        char move[10];
        std::cin >> move;
        Position id = Board::TranslateMove(move, pieces, movementLog->WhitePlays());
        board->MakeMove(id.x, id.y, Pieces, allowCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, movementLog, false, board->CollectedPiece, Board::WhiteEnPassant, Board::BlackEnPassant);
    }
    if (IsKeyPressed(KEY_M))
    {
        if (gameFile->is_open())
        {
            std::string sa;
            getline(*gameFile, sa, ' ');

            std::cout << sa << "\n";
            if (sa[0] == '$')
            {
                //gameFile.close();
                getline(*gameFile, sa, ' ');
                SetPiecesAsDefault(pieces);
            }
            else
            {
                Position id = Board::TranslateMove(sa.c_str(), pieces, movementLog->WhitePlays());
                if (!(id.x == -1 && id.y == -1))
                {
                    board->MakeMove(id.x, id.y, Pieces, allowCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, movementLog, false, board->CollectedPiece, Board::WhiteEnPassant, Board::BlackEnPassant);
                }
            }
        }

    }
    if (IsKeyPressed(KEY_P))
    {
        float* status = NonGraphicalBoard::Status(movementLog->WhitePlays(), pieces, WhitePawn, BlackPawn, WhiteBishop, BlackBishop, WhiteKnight, BlackKnight, WhiteRook, BlackRook, WhiteQueen, BlackQueen, WhiteKing, BlackKing);
        float* MovePointer = predictor.Generate(status);
        printf("Predicted Move: %i, %i\n", (int)MovePointer[0], (int)MovePointer[1]);
        board->DrawMove(MovePointer[0], MovePointer[1]);
        delete[] MovePointer;
    }

    if(showBestMoves)
    {
        for (int i = 0; i < defaultBranchSize; i++)
        {
            board->DrawMove(dataToDraw.bestMoves[i][0], dataToDraw.bestMoves[i][1]);
        }
    }

    movementLog->Draw();

    const char* fen = GetFen(Pieces,allowCastling,movementLog->lastMoveIndex);
    
    float eval = stockfish.getEval(fen);

    delete[] fen;

    //Neural Network
    float* status = NonGraphicalBoard::Status(true, pieces, WhitePawn, BlackPawn, WhiteBishop, BlackBishop, WhiteKnight, BlackKnight, WhiteRook, BlackRook, WhiteQueen, BlackQueen, WhiteKing, BlackKing);
    float* nnEvalPointer = evaluator.Generate(status);
    float nnEval = *nnEvalPointer;
    delete[] nnEvalPointer;

    float* statusNEW = NonGraphicalBoard::Status(true, pieces, WhitePawn, BlackPawn, WhiteBishop, BlackBishop, WhiteKnight, BlackKnight, WhiteRook, BlackRook, WhiteQueen, BlackQueen, WhiteKing, BlackKing);
    float* nnEvalPointerNEW = evaluatorNEW.Generate(statusNEW);
    float nnEvalNEW = *nnEvalPointerNEW;
    delete[] nnEvalPointerNEW;

    float* status2 = NonGraphicalBoard::Status(true, pieces, WhitePawn, BlackPawn, WhiteBishop, BlackBishop, WhiteKnight, BlackKnight, WhiteRook, BlackRook, WhiteQueen, BlackQueen, WhiteKing, BlackKing);
    float* nnEvalPointer2 = evaluator2.Generate(status2);
    float nnEval2 = *nnEvalPointer2;
    delete[] nnEvalPointer2;

    //delete[] status; // THIS Fails, it is deleted in NeuralNetwork::Generate()

    //std::cout << eval << " | " << nnEval << std::endl;

    DrawBar(eval,0);
    DrawBar(nnEval, evalLineHeight);
    DrawBar(nnEvalNEW, evalLineHeight * 2);
    DrawBar(nnEval2, evalLineHeight * 3);

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

    movementLog->DeleteMoves();
    movementLog->lastMoveIndex = 0;

    allowCastling[0] = 1;
    allowCastling[1] = 1;
    allowCastling[2] = 1;
    allowCastling[3] = 1;
}

void Game::Randomize(int seed)
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

BranchEvaluationData<Game::defaultBranchSize> Game::BranchEval(const char* position)
{
    float eval = stockfish.getEval(position);
    float currentEval = eval;

    float maxEvalDiff[defaultBranchSize];
    for (int i = 0; i < defaultBranchSize; i++)
    {
       maxEvalDiff[i] = -(-154 - 154);
    }

    int bestMoves[defaultBranchSize][2] = { -1,-1 };

    for (int From = 0; From < board->totalNumSquares; From++)
    {
        for (int To = 0; To < board->totalNumSquares; To++)
        {
            if (pieces[From] != nullptr)
            {
                if (pieces[From]->IsLegal(pieces, From, To, board, allowCastling) && !(pieces[From]->IsWhite()))
                {
                    Piece** tempBoard = (Piece**)malloc(sizeof(Piece*) * board->totalNumSquares);
                    if (tempBoard == nullptr)
                    {
                        free(tempBoard);
                        return BranchEvaluationData<defaultBranchSize>();
                    }
                    memcpy(tempBoard, pieces, sizeof(Piece*) * board->totalNumSquares);
                    bool* castling = (bool*)malloc(sizeof(bool) * 4);
                    if (castling == nullptr)
                    {
                        free(tempBoard);
                        free(castling);
                        return BranchEvaluationData<defaultBranchSize>();
                    }
                    memcpy(castling, allowCastling, sizeof(bool) * 4);

                    board->MakeMove(From, To, PiecesArray(tempBoard, board->totalNumSquares), castling, BlackDefaultPromotionPiece, BlackDefaultPromotionPiece, nullptr, true, board->CollectedPiece, Board::WhiteEnPassant, Board::BlackEnPassant);

                    const char* fen = GetFen(PiecesArray(tempBoard, board->totalNumSquares), castling,movementLog->lastMoveIndex);
                    float posEval = stockfish.getEval(fen);
                    delete[] fen;

                    for (int i = 0; i < defaultBranchSize; i++)
                    {
                        if (posEval - currentEval < maxEvalDiff[i])// - 11 --(+)10 = -1
                        {
                            bestMoves[i][0] = From;
                            bestMoves[i][1] = To;
                            maxEvalDiff[i] = posEval - currentEval;
                            break;
                        }
                    }
                    //Revert

                    free(tempBoard);
                    free(castling);
                }
            }
        }

    }

    BranchEvaluationData<defaultBranchSize> data = BranchEvaluationData<defaultBranchSize>();

    for (int i = 0; i < defaultBranchSize; i++)
    {
        data.evals[i] = maxEvalDiff[i];
        data.bestMoves[i][0] = bestMoves[i][0];
        data.bestMoves[i][1] = bestMoves[i][1];
    }

    return data;
}

void Game::DrawBar(float num, int offset)
{
    float originalEval = num;

    DrawRectangle(0, screenHeight+offset, screenWidth, evalLineHeight, BLACK);

    num = FastSigmoid(num, 3);

    float evalWidth = num * (screenWidth / 2);//0 - 153 - > 0 - screenWidth/2
    DrawRectangle(0, screenHeight + offset, screenWidth / 2 + evalWidth, evalLineHeight, WHITE);


    if (num >= 0)
    {
        char buffer[5];

        snprintf(buffer, sizeof(buffer), "%f", originalEval);

        DrawText(buffer, 20, screenHeight + offset, 20, BLACK);
    }
    else
    {
        char buffer[5];

        snprintf(buffer, sizeof(buffer), "%f", originalEval * -1);

        DrawText(buffer, screenWidth - MeasureText(buffer, 20) - 20, screenHeight + offset, 20, WHITE);

    }
}

std::fstream* Game::OpenGameFile(const char* path, int gameIndex)
{
    std::fstream* file = new std::fstream();
    file->open(path, std::ios::in);

    int rating[] = { -1,-1 };
    bool ready = false;

    if (file->is_open()) {
        std::string sa;
        while (getline(*file, sa, ' '))
        {
            std::cout << sa << "\n";
            if (sa[0] == '$')
            {
                lineStart:
                sa.erase(0, 1);
                rating[0] = std::stoi(sa);
            }
            else if (rating[1] == -1)
            {
                rating[1] = std::stoi(sa);
                ready = true;
            }
            if (ready)
            {
                if (gameIndex > 0)
                {
                    rating[0] = -1 , rating[1] = -1;
                    gameIndex--;
                    ready = false;
                    do
                    {
                        getline(*file, sa, ' ');      
                    } while (sa[0] != '$');
                    goto lineStart;
                }
                //Position id = Board::TranslateMove(sa.c_str(), pieces, movementLog->WhitePlays());
                //board->MakeMove(id.x, id.y, Pieces, allowCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, movementLog);
                else
                    break;
            }
        }

        //new_file.close();
    }
    return file;
}


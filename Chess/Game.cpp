#include "Game.h"
#include "ActivationMethods.h"

Pawn*   Game::WhitePawn = new Pawn(WHITE);
Pawn*   Game::BlackPawn = new Pawn(BLACK);
Bishop* Game::WhiteBishop = new Bishop(WHITE);
Bishop* Game::BlackBishop = new Bishop(BLACK);
Knight* Game::WhiteKnight = new Knight(WHITE);
Knight* Game::BlackKnight = new Knight(BLACK);
Rook*   Game::WhiteRook = new Rook(WHITE);
Rook*   Game::BlackRook = new Rook(BLACK);
Queen*  Game::WhiteQueen = new Queen(WHITE);
Queen*  Game::BlackQueen = new Queen(BLACK);
King*   Game::WhiteKing = new King(WHITE);
King*   Game::BlackKing = new King(BLACK);

Piece*  Game::WhiteDefaultPromotionPiece = Game::WhiteQueen;
Piece*  Game::BlackDefaultPromotionPiece = Game::BlackQueen;

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

    evaluator.Data();


    //We have an other game instance running!
    ++gameInstances;
}

Game::~Game()
{
    --gameInstances;
    delete board;
    delete BLACK_board;
    delete Board::WhiteEnPassant;//Static variable
    delete Board::BlackEnPassant;//Static variable
    delete movementLog;
    delete sprites;
    if(gameInstances == 0)
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
                    //float currentEval = stockfish.getEval(fen);

                    //int maxEvalDiff = -( - 154 - 154);
                    //int bestMoveIndex;


                    EvalutionType eval_type = {NEURALNETWORK, &evaluator};
                    //BranchEvaluationData<defaultBranchSize> data = BranchEval(eval_type, pieces, allowCastling, movementLog->lastMoveIndex);
                    //EvaluationData eval = Eval<10>(eval_type,(const Piece** const) pieces, allowCastling, movementLog->lastMoveIndex);
                    HeapMovementData eval = Eval(eval_type, pieces, allowCastling, movementLog->lastMoveIndex, 7);
                    

                    //char* movementNotation = board->MovementNotation(Pieces, bestMove[1], bestMove[0], allowCastling);
                    //movementLog->AddMove(movementNotation);
                    //delete[] movementNotation;

                    /*
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
                    */
                    
                    //int bestMove[2] = {data.bestMoves[bestMoveIndex][0], data.bestMoves[bestMoveIndex][1]};
                    int bestMove[2] = {eval.moves[0][0], eval.moves[0][1]};

                    board->MakeMove(bestMove[0], bestMove[1], Pieces, allowCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, movementLog, false, &(board->CollectedPiece), Board::WhiteEnPassant, Board::BlackEnPassant);
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
        board->MakeMove(id.x, id.y, Pieces, allowCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, movementLog, false, &(board->CollectedPiece), Board::WhiteEnPassant, Board::BlackEnPassant);
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
                    board->MakeMove(id.x, id.y, Pieces, allowCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, movementLog, false, &(board->CollectedPiece), Board::WhiteEnPassant, Board::BlackEnPassant);
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

BranchEvaluationData<Game::defaultBranchSize> Game::BranchEval(EvalutionType evaluator, Piece** pieces, bool* allowCastling, int lastMoveIndex, bool bestMove)
{
    float currentEval = GetPosEval(evaluator, pieces, allowCastling, lastMoveIndex);

    float maxPosEval[defaultBranchSize];
    for (int i = 0; i < defaultBranchSize; i++)
    {
       maxPosEval[i] = !(lastMoveIndex % 2 == 0 ^ bestMove) ? -153 : 153;
    }

    int bestMoves[defaultBranchSize][2] = { -1,-1 };

    for (int From = 0; From < Board::totalNumSquares; From++)
    {
        for (int To = 0; To < Board::totalNumSquares; To++)
        {
            if(From == 26 && To == 19)
            {
                int a = 0;
            }
            if (pieces[From] != nullptr)
            {
                if (pieces[From]->IsLegal(pieces, From, To, nullptr, allowCastling) && (!((pieces[From]->IsWhite())   ^   (lastMoveIndex % 2 == 0)))                 )
                {
                    Piece** tempBoard = (Piece**)malloc(sizeof(Piece*) * Board::totalNumSquares);
                    if (tempBoard == nullptr)
                    {
                        free(tempBoard);
                        return BranchEvaluationData<defaultBranchSize>();
                    }
                    memcpy(tempBoard, pieces, sizeof(Piece*) * Board::totalNumSquares);
                    bool* castling = (bool*)malloc(sizeof(bool) * 4);
                    if (castling == nullptr)
                    {
                        free(tempBoard);
                        free(castling);
                        return BranchEvaluationData<defaultBranchSize>();
                    }
                    memcpy(castling, allowCastling, sizeof(bool) * 4);

                    Board::MakeMove(From, To, PiecesArray(tempBoard, Board::totalNumSquares), castling, BlackDefaultPromotionPiece, BlackDefaultPromotionPiece, nullptr, true, nullptr, Board::WhiteEnPassant, Board::BlackEnPassant);

                    float posEval =  GetPosEval(evaluator, tempBoard, castling, lastMoveIndex);

                    for (int i = 0; i < defaultBranchSize; i++)
                    {
                        if(!(lastMoveIndex % 2 == 0 ^ bestMove))//White plays
                        {
                            if (posEval > maxPosEval[i])// - 11 --(+)10 = -1
                            {
                                bestMoves[i][0] = From;
                                bestMoves[i][1] = To;
                                maxPosEval[i] = posEval;
                                break;
                            }
                        }
                        else
                        {
                            if (posEval < maxPosEval[i])// - 11 --(+)10 = -1
                            {
                                bestMoves[i][0] = From;
                                bestMoves[i][1] = To;
                                maxPosEval[i] = posEval;
                                break;
                            }
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
        data.evals[i] = maxPosEval[i];
        data.bestMoves[i][0] = bestMoves[i][0];
        data.bestMoves[i][1] = bestMoves[i][1];
    }

    return data;
}

template <uint depth>
EvaluationData<depth> Game::OptimalEval(EvalutionType evaluator, const Piece** const pieces, const bool* const allowCastling, int lastMoveIndex)
{
    Piece** evalPieces = (Piece**)malloc(Board::totalNumSquares * sizeof(Piece*));
    memcpy(evalPieces, pieces, Board::totalNumSquares * sizeof(Piece*));

    bool* evalCastling = (bool*)malloc(4 * sizeof(bool));
    memcpy(evalCastling, allowCastling, 4 * sizeof(bool));

    BranchEvaluationData<defaultBranchSize> branchEval = BranchEval(evaluator, evalPieces, evalCastling, lastMoveIndex);
    BranchOutputEvaluation<defaultBranchSize> branchEvals[depth - 1];
    BranchOutputEvaluation<defaultBranchSize> previousBranchEval;
    float lastEvals[defaultBranchSize];
    for(int currentDepth = 1; currentDepth < depth; currentDepth++)
    {
        BranchEvaluationData<defaultBranchSize * defaultBranchSize> branchEvalTransition(lastMoveIndex + currentDepth);
        for(int i = 0; i < defaultBranchSize; i++)
        {
            Piece** branchPieces = (Piece**)malloc(Board::totalNumSquares * sizeof(Piece*));
            memcpy(branchPieces, evalPieces, Board::totalNumSquares * sizeof(Piece*));

            bool* branchCastling = (bool*)malloc(4 * sizeof(bool));
            memcpy(branchCastling, evalCastling, 4 * sizeof(bool));


            bool success = true;//Boolen algerbra
            if(currentDepth == 1)
            {
                success = Board::MakeMove(branchEval.bestMoves[i][0], branchEval.bestMoves[i][1], PiecesArray(branchPieces, Board::totalNumSquares), branchCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, nullptr, true, nullptr, Board::WhiteEnPassant, Board::BlackEnPassant);
            }
            else
            {
                HeapMovementData movesToDo = GetTreeMoves(branchEval, branchEvals, i, currentDepth);
                for(int move = 0; move < currentDepth; move++)
                {
                    success *= Board::MakeMove(movesToDo.moves[move][0], movesToDo.moves[move][1], PiecesArray(branchPieces, Board::totalNumSquares), branchCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, nullptr, true, nullptr, Board::WhiteEnPassant, Board::BlackEnPassant);
                }
            if(!success)
                throw;

                /*
                //FIX THIS
                //currebnt depth 1 is taken care here!!!
                success *= Board::MakeMove(branchEval.bestMoves[branchEvals[currentDepth - 2].branchID[i]][0], branchEval.bestMoves[branchEvals[currentDepth - 2].branchID[i]][1], PiecesArray(branchPieces, Board::totalNumSquares), branchCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, nullptr, true, nullptr, Board::WhiteEnPassant, Board::BlackEnPassant);
                for(int idx = 1; idx < currentDepth; idx++)
                {
                    success *= Board::MakeMove(branchEvals[currentDepth - 1 - idx].moves[branchEvals[currentDepth - idx].branchID[idx - 1]][0], branchEvals[currentDepth - 1 - idx].moves[branchEvals[currentDepth - idx].branchID[idx - 1]][1], PiecesArray(branchPieces, Board::totalNumSquares), branchCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, nullptr, true, nullptr, Board::WhiteEnPassant, Board::BlackEnPassant);
                }
                //success = Board::MakeMove(previousBranchEval.moves[i][0], previousBranchEval.moves[i][1], PiecesArray(branchPieces, Board::totalNumSquares), branchCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, nullptr, true, nullptr, Board::WhiteEnPassant, Board::BlackEnPassant);
                */
            }
            if(!success)
                throw;


            BranchEvaluationData<defaultBranchSize> currentBranchEval = BranchEval(evaluator, branchPieces, branchCastling, lastMoveIndex + currentDepth);

            for(int branchItem = 0; branchItem < defaultBranchSize; branchItem++)
            {
                branchEvalTransition.evals[(i * defaultBranchSize) + branchItem] = currentBranchEval.evals[branchItem];
                branchEvalTransition.bestMoves[(i * defaultBranchSize) + branchItem][0] = currentBranchEval.bestMoves[branchItem][0];
                branchEvalTransition.bestMoves[(i * defaultBranchSize) + branchItem][1] = currentBranchEval.bestMoves[branchItem][1];
            }

            free(branchPieces);
            free(branchCastling);
        }

        BranchOutputEvaluation<defaultBranchSize> output;
        float bestEvals[defaultBranchSize];
        for(int i = 0; i < defaultBranchSize;i++)
        {
            bestEvals[i] = (lastMoveIndex + currentDepth) % 2 == 0 ? -153 : 153;
        }
        for(int branch = 0; branch < defaultBranchSize; branch++)
        {
            for(int branchItem = 0; branchItem < defaultBranchSize; branchItem++)
            {
                for(int i = 0; i < defaultBranchSize; i++)
                {
                    if( (lastMoveIndex + currentDepth) % 2 == 0)//White plays
                    {
                        if(branchEvalTransition.evals[(branch * defaultBranchSize) + branchItem] > bestEvals[i])
                        {
                            bestEvals[i] = branchEvalTransition.evals[(branch * defaultBranchSize) + branchItem];
                            output.moves[i][0] = branchEvalTransition.bestMoves[(branch * defaultBranchSize) + branchItem][0];
                            output.moves[i][1] = branchEvalTransition.bestMoves[(branch * defaultBranchSize) + branchItem][1];
                            output.branchID[i] = branch;
                            break;
                        }
                    }
                    else
                    {
                        if(branchEvalTransition.evals[(branch * defaultBranchSize) + branchItem] < bestEvals[i])
                        {
                            bestEvals[i] = branchEvalTransition.evals[(branch * defaultBranchSize) + branchItem];
                            output.moves[i][0] = branchEvalTransition.bestMoves[(branch * defaultBranchSize) + branchItem][0];
                            output.moves[i][1] = branchEvalTransition.bestMoves[(branch * defaultBranchSize) + branchItem][1];
                            output.branchID[i] = branch;
                            break;
                        }
                    }
                }
            }
        }

        if(currentDepth == depth - 1)
        {
            memcpy(lastEvals, bestEvals, sizeof(lastEvals));
        }

        previousBranchEval = output;
        branchEvals[currentDepth - 1] = output;
        //output has the data;
    }

    free(evalPieces);
    free(evalCastling);

    EvaluationData<depth> result;



    float max; 
    int idx_max;
    max = lastEvals[0];
    idx_max = 0.f;
    for(int i = 1; i < defaultBranchSize; i++)
    {
        if( (lastMoveIndex + (depth - 1)) % 2 == 0)//White plays
        {
            if(lastEvals[i] > max)
            {
                idx_max = i;
                max = lastEvals[i];
            }
        }
        else
        {
            if(lastEvals[i] < max)
            {
                idx_max = i;
                max = lastEvals[i];
            }
        }
    }
    result.evaluation = max;
    result.moves[depth - 1][0] = branchEvals[depth - 1 - 1].moves[idx_max][0];
    result.moves[depth - 1][1] = branchEvals[depth - 1 - 1].moves[idx_max][1];

    int branch;
    for(int currentDepth = depth - 2; currentDepth > 0; currentDepth--)
    {
        branch = branchEvals[currentDepth - 1].branchID[currentDepth == depth - 2 ? idx_max : branch];
        result.moves[currentDepth][0] = branchEvals[currentDepth - 1].moves[branch][0];
        result.moves[currentDepth][1] = branchEvals[currentDepth - 1].moves[branch][1];
    }
    result.moves[0][0] = branchEval.bestMoves[branch][0];
    result.moves[0][1] = branchEval.bestMoves[branch][1];

    //HeapMovementData result2 = GetTreeMoves(branchEval, branchEvals, idx_max, depth);

    return result;
}


HeapMovementData Game::Eval(EvalutionType evaluator, Piece** pieces, bool* allowCastling, int lastMoveIndex, uint depth)
{
    //Moddify dirent input from call;

    //bool white = lastMoveIndex % 2 == 0;

    BranchEvaluationData eval = BranchEval(evaluator, pieces, allowCastling, lastMoveIndex);//Take the first eval
    printf("\nMainEval:\n");
    eval.print(pieces, allowCastling);

    BranchOutputEvaluation<defaultBranchSize> branchEvals[depth - 1];


    BranchEvaluationData<defaultBranchSize * defaultBranchSize> evals;//Techically it should be inside the for loop but, anyways...
    
    for(int currentDepth = 1; currentDepth < depth; currentDepth++)
    {//Branch with least offers for oppent
        printf("\nBranch of %s :\n", ((lastMoveIndex + currentDepth) % 2 )== 0 ? "White" : "Black");
        for(int move = 0; move < defaultBranchSize; move++)//Take the 
        {
            Piece** branch_pieces = (Piece**)malloc(Board::totalNumSquares * sizeof(Piece*));
            memcpy(branch_pieces, pieces, Board::totalNumSquares * sizeof(Piece*));

            bool* brach_allow_casling = (bool*)malloc(4 * sizeof(bool));
            memcpy(brach_allow_casling, allowCastling, 4 * sizeof(bool));

            HeapMovementData moves = GetTreeMoves(eval, branchEvals, move, currentDepth);
            for(int i = 0; i < moves.move_depth; i++)
            {
                Board::MakeMove(moves.moves[i].From, moves.moves[i].To, PiecesArray(branch_pieces, Board::totalNumSquares), brach_allow_casling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, nullptr, true, nullptr, Board::WhiteEnPassant, Board::BlackEnPassant);
            }
            BranchEvaluationData current_branch_eval = BranchEval(evaluator, branch_pieces, brach_allow_casling, lastMoveIndex + currentDepth);
            current_branch_eval.print(branch_pieces, brach_allow_casling);

            for(int branchItem = 0; branchItem < defaultBranchSize; branchItem++)
            {
                evals.evals[(move * defaultBranchSize) + branchItem] = current_branch_eval.evals[branchItem];
                evals.bestMoves[(move * defaultBranchSize) + branchItem][0] = current_branch_eval.bestMoves[branchItem][0];
                evals.bestMoves[(move * defaultBranchSize) + branchItem][1] = current_branch_eval.bestMoves[branchItem][1];
            }

            free(branch_pieces);
            free(brach_allow_casling);
        }
        BranchOutputEvaluation previous_branch_eval = GetBestMoves(evals, lastMoveIndex + currentDepth, currentDepth % 2 == 0);
        branchEvals[currentDepth - 1] = previous_branch_eval;

        printf("\nSelected:\n");
        for(int move = 0; move < defaultBranchSize; move++)
        {
            Piece** branch_pieces = (Piece**)malloc(Board::totalNumSquares * sizeof(Piece*));
            memcpy(branch_pieces, pieces, Board::totalNumSquares * sizeof(Piece*));

            bool* brach_allow_casling = (bool*)malloc(4 * sizeof(bool));
            memcpy(brach_allow_casling, allowCastling, 4 * sizeof(bool));

            HeapMovementData moves = GetTreeMoves(eval, branchEvals, move, currentDepth);
            for(int i = 0; i < moves.move_depth; i++)
            {
                Board::MakeMove(moves.moves[i].From, moves.moves[i].To, PiecesArray(branch_pieces, Board::totalNumSquares), brach_allow_casling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, nullptr, true, nullptr, Board::WhiteEnPassant, Board::BlackEnPassant);
            }
            previous_branch_eval.print(branch_pieces, brach_allow_casling, move);


            free(branch_pieces);
            free(brach_allow_casling);
        }
        printf("\n");
    }

    float* lastEvals = GetBestMoveEvals(evals, lastMoveIndex + depth - 1, (depth - 1) & 2 == 0);
    float max; 
    int idx_max;
    max = lastEvals[0];
    idx_max = 0.f;
    for(int i = 1; i < defaultBranchSize; i++)
    {
        if( (lastMoveIndex + (depth - 1)) % 2 == 0)//White plays
        {
            if(lastEvals[i] > max)
            {
                idx_max = i;
                max = lastEvals[i];
            }
        }
        else
        {
            if(lastEvals[i] < max)
            {
                idx_max = i;
                max = lastEvals[i];
            }
        }
    }

    HeapMovementData result = GetTreeMoves(eval, branchEvals, idx_max,depth);

    return result;
}

BranchOutputEvaluation<Game::defaultBranchSize> Game::GetBestMoves(BranchEvaluationData<defaultBranchSize * defaultBranchSize> moves, int lastMoveIndex, bool best)
{
    BranchOutputEvaluation<defaultBranchSize> output;
    float bestEvals[defaultBranchSize];
    for(int i = 0; i < defaultBranchSize;i++)
    {
        bestEvals[i] = !(lastMoveIndex % 2 == 0 ^ best) ? -153 : 153;
    }
    for(int branch = 0; branch < defaultBranchSize; branch++)
    {
        for(int branchItem = 0; branchItem < defaultBranchSize; branchItem++)
        {
            for(int i = 0; i < defaultBranchSize; i++)
            {
                if(!(lastMoveIndex % 2 == 0 ^ best))//White plays
                {
                    if(moves.evals[(branch * defaultBranchSize) + branchItem] > bestEvals[i])
                    {
                        bestEvals[i] = moves.evals[(branch * defaultBranchSize) + branchItem];
                        output.moves[i][0] = moves.bestMoves[(branch * defaultBranchSize) + branchItem][0];
                        output.moves[i][1] = moves.bestMoves[(branch * defaultBranchSize) + branchItem][1];
                        output.branchID[i] = branch;
                        break;
                    }
                }
                else
                {
                    if(moves.evals[(branch * defaultBranchSize) + branchItem] < bestEvals[i])
                    {
                        bestEvals[i] = moves.evals[(branch * defaultBranchSize) + branchItem];
                        output.moves[i][0] = moves.bestMoves[(branch * defaultBranchSize) + branchItem][0];
                        output.moves[i][1] = moves.bestMoves[(branch * defaultBranchSize) + branchItem][1];
                        output.branchID[i] = branch;
                        break;
                    }
                }
            }
        }
    }
    return output;
}

float *Game::GetBestMoveEvals(BranchEvaluationData<defaultBranchSize * defaultBranchSize> moves, int lastMoveIndex, bool best)//Copied code from above function
{
    BranchOutputEvaluation<defaultBranchSize> output;
    float* bestEvals = (float*)malloc(sizeof(float) * defaultBranchSize);
    for(int i = 0; i < defaultBranchSize;i++)
    {
        bestEvals[i] = !(lastMoveIndex % 2 == 0 ^ best) ? -153 : 153;
    }
    for(int branch = 0; branch < defaultBranchSize; branch++)
    {
        for(int branchItem = 0; branchItem < defaultBranchSize; branchItem++)
        {
            for(int i = 0; i < defaultBranchSize; i++)
            {
                if(!(lastMoveIndex % 2 == 0 ^ best))//White plays
                {
                    if(moves.evals[(branch * defaultBranchSize) + branchItem] > bestEvals[i])
                    {
                        bestEvals[i] = moves.evals[(branch * defaultBranchSize) + branchItem];
                        output.moves[i][0] = moves.bestMoves[(branch * defaultBranchSize) + branchItem][0];
                        output.moves[i][1] = moves.bestMoves[(branch * defaultBranchSize) + branchItem][1];
                        output.branchID[i] = branch;
                        break;
                    }
                }
                else
                {
                    if(moves.evals[(branch * defaultBranchSize) + branchItem] < bestEvals[i])
                    {
                        bestEvals[i] = moves.evals[(branch * defaultBranchSize) + branchItem];
                        output.moves[i][0] = moves.bestMoves[(branch * defaultBranchSize) + branchItem][0];
                        output.moves[i][1] = moves.bestMoves[(branch * defaultBranchSize) + branchItem][1];
                        output.branchID[i] = branch;
                        break;
                    }
                }
            }
        }
    }
    return bestEvals;
}

HeapMovementData Game::GetTreeMoves(BranchEvaluationData<defaultBranchSize> base, BranchOutputEvaluation<defaultBranchSize> *phases, int idx, uint depth)
{
    HeapMovementData result(depth);
    int branch;
    if(depth > 1)
    {
        result.moves[depth - 1].From = phases[depth - 1 - 1].moves[idx][0];
        result.moves[depth - 1].To = phases[depth - 1 - 1].moves[idx][1];
    

        branch = phases[depth - 1 - 1].branchID[idx];
    }
    else
    {
        branch = idx;
    }
    for(int currentDepth = depth - 2; currentDepth > 0; currentDepth--)
    {
        result.moves[currentDepth].From = phases[currentDepth - 1].moves[branch][0];
        result.moves[currentDepth].To = phases[currentDepth - 1].moves[branch][1];
        if(currentDepth != 0)
            branch = phases[currentDepth - 1].branchID[branch];
    }

    result.moves[0].From = base.bestMoves[branch][0];
    result.moves[0].To = base.bestMoves[branch][1];

    return result;
}

float Game::GetPosEval(EvalutionType evaluator, Piece** pieces, bool* allowCastling, int lastMoveIndex)
{
    //Wee in the Game namespace
    float eval;
    if(evaluator.type == STOCKFISH)
    {
        const char* fen = GetFen(PiecesArray(pieces, Board::totalNumSquares),allowCastling,lastMoveIndex);
        eval = ((Stockfish*)(evaluator.pos))->getEval(fen);
        delete[] fen;
    }
    else if(evaluator.type == NEURALNETWORK)
    {
        float* status = NonGraphicalBoard::Status(true, pieces, WhitePawn, BlackPawn, WhiteBishop, BlackBishop, WhiteKnight, BlackKnight, WhiteRook, BlackRook, WhiteQueen, BlackQueen, WhiteKing, BlackKing);
        float* nnEvalPointer = ((NeuralNetwork*)evaluator.pos)->Generate(status);
        eval = *nnEvalPointer;
        delete[] nnEvalPointer;
    }
    else
        eval = 0;

    return eval;
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
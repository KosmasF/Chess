#pragma once
#include "NonGraphicalBoard.h"
#include "SocketConnection.h"
#include "NeuralNetwork.h"
#include "BoardCalculations.h"


namespace Batch
{
    float calcBatch(NonGraphicalBoard* board, SocketConnection* stockfish, NeuralNetwork* nn, float mutationRate, float** batchGenerationGradientDescent, int batch
#ifdef ReadFile
        , std::fstream* gameFile, int* fails

#endif
    ,GPU* gpu)
    {
#ifndef ReadFile
        //--------RANDOMIZATION---------
        board->Randomize(rand(), false);
#else
        //--------FILE DATABASE--------
        std::string sa;
        Position id = Position();

        if (gameFile->is_open())
        {
            getline(*gameFile, sa, ' ');

            //std::cout << sa << "\n";
            if (sa[0] == '$')
            {
                //gameFile.close();
                getline(*gameFile, sa, ' ');
                //SetPiecesAsDefault(pieces);
                board->SetPiecesAsDefault(board->pieces);
                goto abort;
            }
            else
            {
                id = Board::TranslateMove(sa.c_str(), board->pieces, board->whitePlays);
                if (!(id.x == -1 && id.y == -1))
                {
                    int tmp = 0;
                    bool success = Board::MakeMove(id.x, id.y, board->Pieces, board->allowCastling, board->WhiteDefaultPromotionPiece, board->BlackDefaultPromotionPiece, nullptr, true, tmp, Board::WhiteEnPassant, Board::BlackEnPassant);
                    if (!success)
                    {
                        (*fails)++;
                        //__debugbreak();
                        //Board::MakeMove(id.x, id.y, board.Pieces, board.allowCastling, board.WhiteDefaultPromotionPiece, board.BlackDefaultPromotionPiece, nullptr, true, tmp, Board::WhiteEnPassant, Board::BlackEnPassant);
                        //board.PrintStatus(true);
                    }
                    board->whitePlays = !board->whitePlays;
                }
                else
                {
                    (*fails)++;
                }
            }
        }
#endif

        //const char* fen = Game::GetFen(board->Pieces, board->allowCastling, 0);
        //float eval = stockfish->getEval(fen);
        //if (!(board->whitePlays))
            //eval *= -1;
        //delete[] fen;

        if (!(id.x == -1 && id.y == -1))
        {
            float* status = board->Status(!(board->whitePlays));
            float* expected = BoardCalculations::FindMoveProbabilities(board->pieces, board->allowCastling,status);

            float* generationStepVector = nn->BackPropagate(expected, board->Status(!(board->whitePlays)), mutationRate);
            float* parallelVector = gpu->BackPropagate(nn->GetAllActivations(status), expected, nn->LayerSize, nn->LayerNum, mutationRate, nn->GetNumberOfWeights());
            batchGenerationGradientDescent[batch] = generationStepVector;
            float* NNoutput = nn->Generate(status);
            float loss = nn->GetLoss(NNoutput, expected);

            free(NNoutput);
            free(expected);

            return loss;

        }
        abort:
            batchGenerationGradientDescent[batch] = nn->EmptyGradient();

        return 1;
    }
}
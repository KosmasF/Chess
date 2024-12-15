#pragma once
#include "NonGraphicalBoard.h"
#include "SocketConnection.h"
#include "NeuralNetwork.h"
#include "BoardCalculations.h"

namespace DFT
{
    extern "C"
    {
        #include "../DFT/dft.h"
    }
}

namespace Batch
{
    inline float calcBatch(NonGraphicalBoard* board, NeuralNetwork* nn, float mutationRate, float** batchGenerationGradientDescent, int batch
#ifdef ReadFile
        , std::fstream* gameFile, int* fails

#endif
    ,GPU* gpu)
    {
#ifndef ReadFile
        //--------RANDOMIZATION---------
        board->Randomize(rand(), false);
       // board->PrintStatus(true);
        {
            float* status = board->Status((board->whitePlays));
            float* out = nn->Generate(status, false);
            float* expected = BoardCalculations::FindMoveProbabilities(board->pieces, board->allowCastling,status);

            //float* generationStepVector = nn->BackPropagate(expected, board->Status(!(board->whitePlays)), mutationRate);
            float* parallelVector = gpu->BackPropagate(nn->GetAllActivations(board->Status((board->whitePlays))), expected, nn->LayerSize, nn->LayerNum, mutationRate, nn->GetNumberOfWeights(), nn->weights, nn->weights_buffer_lookup_table);
            
            
           /* int win = 0;
            int losses = 0;
            int soft_losses = 0;
            for (int i = 0; i < nn->GetNumberOfWeights(); i++)
            {
                if (generationStepVector[i] == parallelVector[i])
                {
                    win++;
                }
                else
                {
                    soft_losses++;
                    if (abs(generationStepVector[i] - parallelVector[i]) > 1)
                    {
                        std::cout << parallelVector[i] << " | " << generationStepVector[i] << std::endl;
                        losses++;
                    }
                }
            }

            printf("Wins: %d, Losses: %d, Soft Losses: %d\n", win, losses, soft_losses);*/
            
            batchGenerationGradientDescent[batch] = parallelVector;//generationStepVector
            float* NNoutput = nn->Generate(status);
            float loss = nn->GetLoss(NNoutput, expected);

            free(NNoutput);
            free(expected);
            //free(parallelVector);

            return loss;

        }
        abort:
            batchGenerationGradientDescent[batch] = nn->EmptyGradient();

        return 1;
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

        //const char* fen = Game::GetFen(board->Pieces, board->allowCastling, 0);
        //float eval = stockfish->getEval(fen);
        //if (!(board->whitePlays))
            //eval *= -1;
        //delete[] fen;

        if (!(id.x == -1 && id.y == -1))
        {
            float* status = board->Status(!(board->whitePlays));
            float* expected = BoardCalculations::FindMoveProbabilities(board->pieces, board->allowCastling,status);

            // ;) float* generationStepVector = nn->BackPropagate(expected, board->Status(!(board->whitePlays)), mutationRate);
            float* parallelVector = gpu->BackPropagate(nn->GetAllActivations(board->Status(!(board->whitePlays))), expected, nn->LayerSize, nn->LayerNum, mutationRate, nn->GetNumberOfWeights(), nn->weights, nn->weights_buffer_lookup_table);
            
            
           /* int win = 0;
            int losses = 0;
            int soft_losses = 0;
            for (int i = 0; i < nn->GetNumberOfWeights(); i++)
            {
                if (generationStepVector[i] == parallelVector[i])
                {
                    win++;
                }
                else
                {
                    soft_losses++;
                    if (abs(generationStepVector[i] - parallelVector[i]) > 1)
                    {
                        std::cout << parallelVector[i] << " | " << generationStepVector[i] << std::endl;
                        losses++;
                    }
                }
            }

            printf("Wins: %d, Losses: %d, Soft Losses: %d\n", win, losses, soft_losses);*/
            
            batchGenerationGradientDescent[batch] = parallelVector;//generationStepVector
            float* NNoutput = nn->Generate(status);
            float loss = nn->GetLoss(NNoutput, expected);

            free(NNoutput);
            free(expected);
            //free(parallelVector);

            return loss;

        }
        abort:
            batchGenerationGradientDescent[batch] = nn->EmptyGradient();

        return 1;

        #endif
    }

    float calcVowelBranch(NeuralNetwork* nn, float mutationRate, float** batchGenerationGradientDescent, int batch   ,GPU* gpu);
    float calcVowelBranchTransformed(NeuralNetwork* nn, float mutationRate, float** batchGenerationGradientDescent, int batch   ,GPU* gpu);
}
//#define SERVER_TEST

#include "Game.h"
#include "NonGraphicalBoard.h"
#include "NeuralNetwork.h"
#include <thread>
#include <intrin.h>

#pragma warning (disable : 4996)
#include <stdio.h>

#define INTERNAL_SERVER

const bool graphical = 0;

int outputToMove(float x , float y)
{
    return (int)(x*7 + ((y*7)*8));
}

void LaunchStockfish()
{
    printf("Launching Stockfish..\n");
    bool result = system("Stockfish.exe");
    if (result)
    {
        printf("Stockfish launched successfully!\n");
    }
}

int main(int argc, char** argv)
{
#ifdef INTERNAL_SERVER
    std::thread stockfishThread(LaunchStockfish);
#endif

    if (graphical)
    {

        Game* game = new Game(argc,argv);

        while (!WindowShouldClose())
        {
            game->Update();
        }


        delete game;
    }
    else
    {
//#define reinforced
#ifdef reinforced
            //bool running = true;

            //VARIABLES
            const int population = 1;

            int networkSizes[4] = { 64,100,25,4 };
            float (*activationMethods[])(float) = { None,None,None };

            //SETUP
            printf("Setup\n");
            NonGraphicalBoard** boards = new NonGraphicalBoard * [population];
            NeuralNetwork* nnsWhite[population];
            NeuralNetwork* nnsBlack[population];

            for (int i = 0; i < population; i++)
            {
                boards[i] = new NonGraphicalBoard();
                nnsWhite[i] = new NeuralNetwork(networkSizes, 4, activationMethods);
                nnsBlack[i] = new NeuralNetwork(networkSizes, 4, activationMethods);
            }



            //TRAINING VARIABLES
            printf("Starting Training...\n");
            int epochs = 0;
            float mutationRate = 0.05f; // 5%

            while (epochs > 0)
            {
                for (int i = 0; i < population; i++)
                {
                    //DECLARATIONS
                    float* output;
                    bool move;

                    //-------------MOVES--------------\\


                    //WHITE
                    output = nnsWhite[i]->Generate(boards[i]->Status(true));

                    //std::cout << outputToMove(output[0],output[1]) << " | " << outputToMove(output[2],output[3]) << std::endl;
                    move = boards[i]->Move(outputToMove(output[0], output[1]), outputToMove(output[2], output[3]), true);
                    if (!move)
                    {
                        //printf("White Failed!\n");
                        //Inherit from black

                        void* winnerData = nnsBlack[i]->Data();
                        nnsWhite[i]->Load(winnerData);
                        free(winnerData);
                        continue;
                    }


                    //BLACK
                    output = nnsBlack[i]->Generate(boards[i]->Status(false));

                    //std::cout << outputToMove(output[0], output[1]) << " | "<< outputToMove(output[2], output[3]) << std::endl;
                    move = boards[i]->Move(outputToMove(output[0], output[1]), outputToMove(output[2], output[3]), false);
                    if (!move)
                    {
                        //printf("Black Failed!\n");
                        //Inherit from black

                        void* winnerData = nnsWhite[i]->Data();
                        nnsBlack[i]->Load(winnerData);
                        free(winnerData);
                        std::cin.get();
                        continue;
                    }


                    delete[] output;

                }


                //const char* path = "networks/data.nn";

                //nnWhite.LoadFromDisk(path);
                //output = nnWhite.Generate(board.Status(true));
                //std::cout << output[0] << std::endl;

            EndLoop:
                //---------MUTATION--------------\\
                    
                if (epochs > 1)
                {
                    //Do opt
                    //nnWhite.Mutate(mutationRate);
                    //nnBlack.Mutate(mutationRate);
                }





                //----------LOOP FLOW------------\\
                
                    //running = false;
                epochs--;
                std::cout << epochs << '\n';

            }
            const char* path = "networks/test.nn";
            //nnWhite.Save(path);

            for (int i = 0; i < population; i++)
            {
                delete boards[i];
                delete nnsWhite[i];
                delete nnsBlack[i];
            }


#else
    //#define ReadFile
    #ifdef ReadFile
            //SET FILE READER
            std::fstream gameFile;

            gameFile.open("games/output.pgn", std::ios::in);

            int lines = 0;
            int rating[] = { -1,-1 };
            bool ready = false;

            if (gameFile.is_open()) {
                std::string sa;
                while (getline(gameFile, sa, ' '))
                {
                    //std::cout << sa << "\n";
                    if (sa[0] == '$')
                    {
                        if (0 < lines)
                            break;
                        lines++;
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
                        //Position id = Board::TranslateMove(sa.c_str(), pieces, movementLog->WhitePlays());
                        //board->MakeMove(id.x, id.y, Pieces, allowCastling, WhiteDefaultPromotionPiece, BlackDefaultPromotionPiece, movementLog);
                        break;
                    }
                }

                //new_file.close();
            }
    #endif

            //END SET UP

            time_t startTime = time(NULL);
            srand(startTime);

            printf("Setup\n");

            int batchSize = 16;
            int batches = 1000;

            //int networkSizes[] = { 64,256,1 };
            //float (*activationMethods[])(float) = {None,None,None,None};

            NeuralNetwork nn = NeuralNetwork("networks/testedNonRandom3LayersBIG.nn");
            NonGraphicalBoard board;


            //const char* path = "networks/testEvaluatorNonRandomWeights.nn";
            //nn.LoadFromDisk(path);

            SocketConnection stockfish;
            stockfish.Setup(argc,argv);

            int fails = 0;

            float mutationRate = 0.001f;

            printf("Training...\n");

            int iterations = 0;
            while (iterations < batches)
            {
                float* output = nullptr;
                float eval = 0;
                float** batchGenerationGradientDescent = new float*[batchSize];

                for (int batch = 0; batch < batchSize; batch++)
                {

            #ifndef ReadFile
                    //--------RANDOMIZATION---------
                    board.Randomize(rand(), false);
            #else
                    //--------FILE DATABASE--------

                    if (gameFile.is_open())
                    {
                        std::string sa;
                        getline(gameFile, sa, ' ');

                        //std::cout << sa << "\n";
                        if (sa[0] == '$')
                        {
                            //gameFile.close();
                            getline(gameFile, sa, ' ');
                            //SetPiecesAsDefault(pieces);
                            board.SetPiecesAsDefault(board.pieces);
                        }
                        else
                        {
                            Position id = Board::TranslateMove(sa.c_str(), board.pieces, board.whitePlays);
                            if (!(id.x == -1 && id.y == -1))
                            {
                                int tmp = 0;
                                bool success = Board::MakeMove(id.x, id.y, board.Pieces, board.allowCastling, board.WhiteDefaultPromotionPiece, board.BlackDefaultPromotionPiece, nullptr, true, tmp, Board::WhiteEnPassant, Board::BlackEnPassant);
                                if (!success)
                                {
                                    fails++;
                                    //__debugbreak();
                                    //Board::MakeMove(id.x, id.y, board.Pieces, board.allowCastling, board.WhiteDefaultPromotionPiece, board.BlackDefaultPromotionPiece, nullptr, true, tmp, Board::WhiteEnPassant, Board::BlackEnPassant);
                                    //board.PrintStatus(true);
                                }
                                board.whitePlays = !board.whitePlays;
                            }
                        }
                    }
                #endif

                    //output = nn.Generate(board.Status(board.whitePlays));
                    //delete[] output;

                    const char* fen = Game::GetFen(board.Pieces, board.allowCastling, 0);
                    eval = stockfish.getEval(fen);
                    if (!board.whitePlays)
                        eval *= -1;
                    delete[] fen;

                    float* generationStepVector = nn.BackPropagate(&eval, board.Status(true), mutationRate);
                    batchGenerationGradientDescent[batch] = generationStepVector;
                }

                output = nn.Generate(board.Status(board.whitePlays));

                float* batchGradientDescent = nn.AverageWeightVector(batchGenerationGradientDescent,batchSize);
                nn.AddToWeights(batchGradientDescent);
                for (int i = 0; i < batchSize; i++)
                    free(batchGenerationGradientDescent[i]);
                delete[] batchGradientDescent;
                delete[] batchGenerationGradientDescent;

                float loss = nn.GetLoss(output, &eval);
                printf("Iteration %d , loss: %f \n", iterations, loss);
                //if (iterations == 383)  __debugbreak();

                delete[] output;

                iterations++;
            }

            printf("Closing...\n");
            //char path[256];
            //std::cin >> path;
            //const char* path = "networks/nnRe-evalInMasterGamesErrorCorrection.nn";
            //nn.Save(path);

            printf("Training started in %i and ended, duration: %f\n",(int)startTime, (float)(time(NULL) - startTime));
            printf("Database fails: %i\n", fails);
        }
#endif






#ifdef INTERNAL_SERVER
    stockfishThread.join();
#endif

    return 0;
}

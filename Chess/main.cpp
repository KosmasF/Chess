//#define SERVER_TEST

#include "Game.h"
#include "NonGraphicalBoard.h"
#include "NeuralNetwork.h"
#include <thread>

#pragma warning (disable : 4996)
#include <stdio.h>

#define INTERNAL_SERVER

const bool graphical = 1;

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
        bool reinforced = false;
        if (reinforced)
        {
            //bool running = true;

            //VARIABLES
            const int population = 1000;

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


        }
        else
        {
            time_t startTime = time(NULL);
            srand(startTime);

            printf("Setup\n");

            int batchSize = 10;
            int batches = 1000;

            int networkSizes[] = { 64,256,1 };
            float (*activationMethods[])(float) = {None,None,None,None};

            NeuralNetwork nn = NeuralNetwork(networkSizes, sizeof(networkSizes)/sizeof(int), activationMethods);
            NonGraphicalBoard board;


            //const char* path = "networks/testEvaluatorNonRandomWeights.nn";
            //nn.LoadFromDisk(path);

            SocketConnection stockfish;
            stockfish.Setup(argc,argv);

            float mutationRate = 0.0005f;

            printf("Training...\n");

            int iterations = 0;
            while (iterations < batches)
            {
                float* output = nullptr;
                float eval = 0;
                float** batchGenerationGradientDescent = new float*[batchSize];

                for (int batch = 0; batch < batchSize; batch++)
                {
                    board.Randomize(rand(), false);

                    output = nn.Generate(board.Status(board.whitePlays));

                    const char* fen = Game::GetFen(board.Pieces, board.allowCastling, 0);
                    eval = stockfish.getEval(fen);
                    if (!board.whitePlays)
                        eval *= -1;
                    delete[] fen;

                    float* generationStepVector = nn.BackPropagate(&eval, board.Status(true), mutationRate);
                    batchGenerationGradientDescent[batch] = generationStepVector;
                }

                float* batchGradientDescent = nn.AverageWeightVector(batchGenerationGradientDescent,batchSize);
                nn.AddToWeights(batchGradientDescent);
                for (int i = 0; i < batchSize; i++)
                    free(batchGenerationGradientDescent[i]);
                delete[] batchGradientDescent;
                delete[] batchGenerationGradientDescent;


                float loss = nn.GetLoss(output, &eval);
                printf("Iteration %d , loss: %f \n", iterations, loss);

                delete[] output;

                iterations++;
            }

            printf("Closing...\n");
            //char path[256];
            //std::cin >> path;
            const char* path = "networks/test.nn";
            nn.Save(path);

            printf("Training started in %i and ended, duration: %f\n",(int)startTime, (float)(time(NULL) - startTime));
        }
    }






#ifdef INTERNAL_SERVER
    stockfishThread.join();
#endif

    return 0;
}

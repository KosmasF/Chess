#include "Game.h"
#include <thread>
#include "NonGraphicalBoard.h"
#include "NeuralNetwork.h"

#pragma warning (disable : 4996)
#include <stdio.h>

const bool graphical = false;

int outputToMove(float x , float y)
{
    return (int)(x*7 + ((y*7)*8));
}

int main()
{
    if (graphical)
    {
        Game* game = new Game();

        while (!WindowShouldClose())
        {
            game->Update();
        }

        delete game;
    }
    else
    {// This is the neural network input!
        NonGraphicalBoard board = NonGraphicalBoard();
        int networkSizes[3] = { 64,5,4 };

        NeuralNetwork nnWhite = NeuralNetwork(networkSizes,3);
        NeuralNetwork nnBlack = NeuralNetwork(networkSizes,3);

        float* output;

        output = nnWhite.Generate(board.Status(true));

        //std::cout << outputToMove(output[0],output[1]) << " | " << outputToMove(output[2],output[3]) << std::endl;
        bool moved = board.Move(outputToMove(output[0], output[1]), outputToMove(output[2], output[3]), true);

        output = nnBlack.Generate(board.Status(false));

        //std::cout << outputToMove(output[0], output[1]) << " | "<< outputToMove(output[2], output[3]) << std::endl;
        moved = board.Move(outputToMove(output[0], output[1]), outputToMove(output[2], output[3]), false);


        void* data = nnWhite.Data();
        int dataSize = ((int*)data)[0];


        const char* path = "data.nn";

        FILE* file = fopen("networks/data.nn", "wb");
        size_t r1 = fwrite(data,dataSize,1,file);
        std::cout << "Saved file as: " << path << std::endl;
        fclose(file);

        free(data);
        delete[] output;
    }

    return 0;
}
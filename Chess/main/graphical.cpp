#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "NonGraphicalBoard.h"
#include "NeuralNetwork.h"
#include "Graph.h"
#include "GPU.h"
#include "Batch.h"

GPU gpu;

int main(int argc, char** argv)
{
    Game* game = new Game(argc,argv);

    while (!WindowShouldClose())
    {
        game->Update();
    }

    delete game;

    return 0;
}
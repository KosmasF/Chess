#include "Game.h"
#include <thread>
#include "NonGraphicalBoard.h"

const bool graphical = false;

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
    {
        NonGraphicalBoard board = NonGraphicalBoard();

        int* status = board.Status(true);// This is the neural network input!

        board.PrintStatus(true);

        std::cout << board.Move(1,8+8,true) << '\n' << std::endl;

        board.PrintStatus(true);

        int output[2] = { 0,0 };//Get the NN output

        delete status;
    }

    return 0;
}
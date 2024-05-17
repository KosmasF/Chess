#include "Game.h"
#include "raylib.h"


int main() {
    

    Game* game = new Game();

    while (!WindowShouldClose())
    {

        BeginDrawing();


        ClearBackground(RAYWHITE);

        game->Update();
    }

    delete game;
    return 0;
}
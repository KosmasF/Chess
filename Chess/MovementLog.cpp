#include "MovementLog.h"

void MovementLog::DrawHeader()
{
    DrawText(header,

        ScreenHeight + LogSize / 2 - MeasureText(header, fontSize) / 2,

        20, fontSize, WHITE);
}

void MovementLog::DrawBackground()
{
    DrawRectangle(ScreenHeight, 0, LogSize, ScreenHeight, lightBlack);
    DrawRectangle(ScreenHeight + 10, 10, LogSize - 20, ScreenHeight - 20, BLACK);
}

MovementLog::MovementLog(int logSize , int screenHeight)
{
    LogSize = logSize;
    ScreenHeight = screenHeight;

    LogCenterPos = screenHeight + (logSize / 2);

    for (int i = 0; i < maxMoves; i++)
    {
        moves[i] = nullptr;
    }
}

void MovementLog::Draw()
{
    DrawBackground();
    DrawHeader();

    int pos = 50;

    for (int i = fistNumToDraw; i < lastMoveIndex; i+=2)
    {
        int buffer = 0;

        int idx = i+1;
        idx /= 2;
        idx++;
        char num[3];
        int c = 0;
        while (idx > 0)
        {
            int digit = idx % 10;
            idx /= 10;
            num[c] = (char)(digit + one);
            c++;
        }

        char reversedArray[4];
        for (int h = 0; h < c; h++)
        {
            reversedArray[h] = num[c - h -1 ];
        }
        reversedArray[c] = 0;

        DrawText(reversedArray, ScreenHeight + 20, pos, fontSize, WHITE);
        buffer += MeasureText(reversedArray, fontSize);
        buffer += 3;

        DrawText(".", ScreenHeight + 20 + buffer, pos, fontSize, WHITE);
        buffer += MeasureText(".", fontSize);

        //White Moves
        int moveLength = MeasureText(moves[i], fontSize);
        int moveSizeLeftEmpty = LogSize / 2 - moveLength;
        DrawText(moves[i], ScreenHeight + 20 + (moveSizeLeftEmpty / 2), pos, fontSize, WHITE);

        //BlackMoves
        moveLength = MeasureText(moves[i+1], fontSize);
        moveSizeLeftEmpty = LogSize / 2 - moveLength;
        DrawText(moves[i+1], LogCenterPos + (moveSizeLeftEmpty / 2), pos, fontSize, WHITE);


        pos += 25;
    }

}

MovementLog::~MovementLog()
{
    DeleteMoves();
}

void MovementLog::DeleteMoves()
{
    for (int i = 0; i < maxMoves; i++)
    {
        delete moves[i];
        moves[i] = nullptr;
    }
}

void MovementLog::AddMove(const char* move)
{
    moves[lastMoveIndex] = move;
    if (lastMoveIndex > 18*2)
        if(WhitePlays())
            fistNumToDraw+=2;
    lastMoveIndex++;
}

bool MovementLog::WhitePlays()
{
    return lastMoveIndex%2 == 0;
}

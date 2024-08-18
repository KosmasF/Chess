#pragma once

#include <stdlib.h>
#include <stdio.h>

/*    if (!successful)
    {
        printf("Connected Successfully with Stockfish server!\n");
    }
    else
    {
        printf("Failed connection with Stockfish");
    }*/

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27015

class SocketConnection
{
public:
    SocketConnection();
    bool Setup(int argc, char** argv);
    //bool Commune();
    ~SocketConnection();
    float getEval(const char* fen);
private:
    int clientSocket;
};

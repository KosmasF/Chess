#pragma once


#define WIN32_LEAN_AND_MEAN

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

#ifdef _MSVC_LANG //Check if we are using c++ with visual studio

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#endif // _MSVC_LANG


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

typedef unsigned long long UINT_PTR;
typedef UINT_PTR SOCKET;



class SocketConnection
{
public:
    SocketConnection();
    bool Setup(int argc, char** argv);
    bool Commune();
    ~SocketConnection();
    float getEval(const char* fen);
private:
    SOCKET ConnectSocket;
    struct addrinfo* result;
    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
};

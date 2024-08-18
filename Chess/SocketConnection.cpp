#include "SocketConnection.h"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


bool SocketConnection::Setup(int argc, char** argv)
{
	 // creating socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(DEFAULT_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // sending connection request
    connect(clientSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));
}

/*bool SocketConnection::Commune()
{
    //----------------------------Start Communication--------------------------------

    // Send an initial buffer
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %d\n", iResult);

    //-----------------------------Communication Loop--------------------------------

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            
            iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
            if (iResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iResult);
        }
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while (iResult > 0);


    //--------------------------------------SHUTDOWN----------------------------------

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}*/

SocketConnection::~SocketConnection()
{
  close(clientSocket);
}

float SocketConnection::getEval(const char* fen)
{
    //----------------------------Start Communication--------------------------------//

    // Send an initial buffer
    //iResult = send(ConnectSocket, "Eval", (int)strlen(sendbuf), 0);//Len = 5;
	send(clientSocket, "Eval", strlen("Eval"), 0);

    //printf("Eval Request Sent.\n");

	char recvbuf[DEFAULT_BUFLEN] =  {0}; 
    recv(clientSocket, recvbuf, sizeof(recvbuf), 0);//MAKE SAFETY MEASURES
    if (recvbuf[0] == 'R')
    {
        send(clientSocket, fen, (int)strlen(fen), 0);
    }
    else
    {
        printf("Server is busy, communication cancelled!\n");
        return 0;
    }

    //-----------------------------Communication Loop--------------------------------//

    int iResult;
    // Receive until the peer closes the connection
    do {
        iResult = recv(clientSocket, recvbuf, sizeof(recvbuf), 0);
        if (iResult > 0) {
            //printf("Eval received\n");
            goto SHUTDOWN;
        }
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: \n");

    } while (iResult > 0);

    SHUTDOWN:
    //--------------------------------------SHUTDOWN----------------------------------//

    return *((float*)recvbuf);
}

SocketConnection::SocketConnection()
{

}

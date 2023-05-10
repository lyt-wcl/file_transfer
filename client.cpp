#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5851);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to server: " << WSAGetLastError() << '\n';
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    char buffer[1024] = "Hello, server!";
    if (send(clientSocket, buffer, strlen(buffer), 0) == SOCKET_ERROR)
    {
        std::cerr << "Failed to send data: " << WSAGetLastError() << '\n';
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    memset(buffer, 0, sizeof(buffer));
    int numBytes = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (numBytes == SOCKET_ERROR)
    {
        std::cerr << "Failed to receive data: " << WSAGetLastError() << '\n';
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    else if (numBytes == 0)
    {
        std::cerr << "Connection closed by server\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Received from server: " << buffer << '\n';

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

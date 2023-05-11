#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib") //加载ws2_32库

using namespace std;
DWORD WINAPI clientThread(LPVOID lpParam)
{
    SOCKET clientSocket = *(SOCKET*)lpParam;
    
    // Receive client messages
    char buffer[1024];
    int recvSize;
    
    recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (recvSize > 0) {
        buffer[recvSize] = '\0';
        std::cout << "Received message: " << buffer << std::endl;
    }
    else if (recvSize == 0) {
        std::cout << "Client disconnected." << std::endl;
    }
    else {
        std::cerr << "Failed to receive message: " << WSAGetLastError() << std::endl;
    }
    
    std::string response = "success";
    if (send(clientSocket, response.c_str(), response.size(), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send data: " << WSAGetLastError() << std::endl;
    }

    closesocket(clientSocket);
    return 0;
}

int main () 
{
    //initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) 
    {
        cout << "WSAStartup failed: " << result << endl;
        return 1;
    }

    // create socket
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        cout << "Failed to create socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // bind socket
    sockaddr_in serverAddress; //sockaddr_in is a structure used to store IPv4 address information
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5851); //port
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    result = bind(listenSocket, (SOCKADDR *)&serverAddress, sizeof(serverAddress));
    if (result == SOCKET_ERROR)
    {
        cout << "Failed to bind socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // open listening
    //SOMAXCONN is an integer that represents the system's default maximum connection queue length
    result = listen(listenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) 
    {
        cout << "Failed to listen: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }
    else 
    {
        cout << "listening..." << endl;
    }

    // accept the bind of client
    
    sockaddr_in clientAddress;
    int addrLen = sizeof(clientAddress);
    
    while (true)
    {
        SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddress, &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Failed to accept client: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        HANDLE threadHandle = CreateThread(NULL, 0, clientThread, &clientSocket, 0, NULL);
        if (threadHandle == NULL)
        {
            std::cerr << "Failed to create thread." << std::endl;
            closesocket(clientSocket);
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        // wait the thread end;
        WaitForSingleObject(threadHandle, INFINITE);

        // close the thread's handle
        CloseHandle(threadHandle);
        closesocket(clientSocket);
    }

    // close bind
    
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
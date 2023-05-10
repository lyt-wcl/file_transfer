#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") //加载ws2_32库

using namespace std;
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

    // accept the bind of client
    SOCKET clientSocket;
    sockaddr_in clientAddress;
    int addrLen = sizeof(clientAddress);
    clientSocket = accept(listenSocket, (SOCKADDR *)&clientAddress, &addrLen);
    if (clientSocket == INVALID_SOCKET) 
    {
        cout << "Failed to accept client: " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Receive client messages
    char buffer[1024];
    int recvSize;
    
    recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (recvSize > 0) {
        buffer[recvSize] = '\0';
        cout << "Received message: " << buffer << endl;
    }
    else if (recvSize == 0) {
        cout << "Client disconnected." << endl;
    }
    else {
        cout << "Failed to receive message: " << WSAGetLastError() << endl;
    }
    string str = "success";
    if (send(clientSocket, str.c_str(), str.size(), 0) == SOCKET_ERROR)
    {
        std::cerr << "Failed to send data: " << WSAGetLastError() << '\n';
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // close bind
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
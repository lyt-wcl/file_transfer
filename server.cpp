#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <conio.h>
#include <fstream>
#pragma comment(lib, "ws2_32.lib") //加载ws2_32库
#define BUFFER_SIZE 1024
using namespace std;
DWORD WINAPI clientThread(LPVOID lpParam)
{
    SOCKET clientSocket = *(SOCKET*)lpParam;
    
    // Receive client messages
    cout << "Client connected." << endl;
    // 接收文件大小
    int fileSize;
    int bytesReceived = recv(clientSocket, (char*)&fileSize, sizeof(fileSize), 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cout << "Failed to receive file size." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    } 
    else {
        cout << "file size:" << fileSize << endl;
    }
    // 接收文件内容并保存到磁盘
    ofstream file("test/test.mp3",ios::binary | ios::app); // 追加写入模式，可以根据需要更改文件名

    char buffer[BUFFER_SIZE];
    int count = 0;
    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        file.write(buffer, bytesReceived);
        count++;
    }
    cout << count << endl;

    file.close();

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
            cerr << "Failed to accept client: " << WSAGetLastError() << endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        HANDLE threadHandle = CreateThread(NULL, 0, clientThread, &clientSocket, 0, NULL);
        if (threadHandle == NULL)
        {
            cerr << "Failed to create thread." << endl;
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
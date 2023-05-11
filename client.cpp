#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#define BUFFER_SIZE 1024
using namespace std;

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        cerr << "Failed to create socket: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5851);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerr << "Failed to connect to server: " << WSAGetLastError() << '\n';
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    ifstream file("test.mp3", ios::binary); // 替换为待发送文件的路径
    if (!file) {
        cout << "Failed to open file." << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    

    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // 发送文件大小
    if (send(clientSocket, (char*)&fileSize, sizeof(fileSize), 0) == SOCKET_ERROR) {
        std::cout << "Failed to send file size." << std::endl;
        file.close();
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // 设置缓冲区
    const int bufferSize = 1024;
    char buffer[bufferSize];
    int count = 0;
    while (!file.eof()) {
        file.read(buffer, bufferSize);
        int bytesRead = file.gcount();
        int bytesSent = send(clientSocket, buffer, bytesRead, 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cout << "Failed to send data." << std::endl;
            file.close();
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        count++;
    }
    cout << count << endl;

    file.close();
    closesocket(clientSocket);
    WSACleanup();

    cout << "File sent successfully." << endl;

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

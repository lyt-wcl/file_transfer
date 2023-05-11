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

    ifstream file("test.bin", ios::binary); // 替换为待发送文件的路径
    if (!file) {
        cout << "Failed to open file." << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    char buffer[BUFFER_SIZE];
    int bytesRead;
    int totalSent = 0; // 用于跟踪已发送的字节数

    while ((bytesRead = file.read(buffer, BUFFER_SIZE).gcount()) > 0) {
        int bytesSent = send(clientSocket, buffer, bytesRead, 0);
        if (bytesSent == SOCKET_ERROR) {
            cout << "Failed to send data." << endl;
            file.close();
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        totalSent += bytesSent;

        cout << "Sent: " << totalSent << " bytes" << endl;
    }

    file.close();
    closesocket(clientSocket);
    WSACleanup();

    cout << "File sent successfully." << endl;

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

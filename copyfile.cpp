#include <iostream>
#include <fstream>

int main() {
    std::ifstream sourceFile("test.mp3", std::ios::binary); // 替换为源 MP3 文件的路径
    std::ofstream destinationFile("destination.mp3", std::ios::binary); // 替换为目标 MP3 文件的路径

    if (!sourceFile) {
        std::cout << "Failed to open source file." << std::endl;
        return 1;
    }

    if (!destinationFile) {
        std::cout << "Failed to create destination file." << std::endl;
        sourceFile.close();
        return 1;
    }

    // 获取文件大小
    sourceFile.seekg(0, std::ios::end);
    int fileSize = sourceFile.tellg();
    sourceFile.seekg(0, std::ios::beg);

    // 设置缓冲区
    const int bufferSize = 1024;
    char buffer[bufferSize];

    // 逐块复制文件内容
    while (!sourceFile.eof()) {
        sourceFile.read(buffer, bufferSize);
        int bytesRead = sourceFile.gcount();

        destinationFile.write(buffer, bytesRead);
    }

    sourceFile.close();
    destinationFile.close();

    std::cout << "File copied successfully." << std::endl;

    return 0;
}

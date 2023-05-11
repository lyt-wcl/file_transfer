#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
using namespace std;
vector<string> GetFilesInDirectory(const string& directory) {
    vector<string> files;

    string searchPath = directory + "*"; // 搜索路径为目录下的所有文件
    
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // 忽略目录
                continue;
            }
            string filename = findData.cFileName;
            files.push_back(filename);
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
    }

    return files;
}

int main() {
    string directory = "test\\"; // 替换为要打开的目录的路径

    vector<string> files = GetFilesInDirectory(directory);

    for (const string& filename : files) {
        cout << filename << endl;
    }

    return 0;
}

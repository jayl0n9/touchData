#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdio>
#include <cassert>
#include <windows.h>
#include <sstream>
#include <iomanip>
using namespace std;

struct FileTimes {
    FILETIME creationTime;
    FILETIME lastAccessTime;
    FILETIME lastWriteTime;
};

void showHelp();
bool parseArgs(int argc, char* argv[], string& path_f, string& path_r, string& path_d);
bool setFileTime(const string& targetPath, const FileTimes& fileTimes);
FileTimes getFileTimes(const string& filePath);
bool parseDateTime(const std::string& dateTimeStr, FILETIME& fileTime);
void SetFileTimesTo(FILETIME* time, struct FileTimes* fileTimes);


int main(int argc, char* argv[]) {
    string path_f, path_r, path_d;

    if (!parseArgs(argc, argv, path_f, path_r, path_d)) {
        showHelp();
        return 1;
    }


    if (!path_r.empty()) {
        FileTimes sourceFileTimes = getFileTimes(path_r);
        if (!setFileTime(path_f, sourceFileTimes)) {
            cerr << "error：Failed to set file time。" << endl;
            return 1;
        }
        cout << "The time of  " << path_f << " is successfully set to the time of " << path_r << endl;
        return 0;
    }
    else if (!path_d.empty()) {
        FILETIME fileTime;

        if (!parseDateTime(path_d, fileTime)) {
            std::cerr << "Failed to convert datetime to FILETIME." << std::endl;
        }
        struct FileTimes fileTimes;
        SetFileTimesTo(&fileTime, &fileTimes);

        if (!setFileTime(path_f, fileTimes)) {
            cerr << "error：Failed to set file time。" << endl;
            return 1;
        }
        cout << "The time of  " << path_f << " is successfully set to " << path_d << endl;
        return 0;
    }
    else {
        std::cout << "Directory of file: " << path_f << std::endl;
        return 0;
    }
    return 0;
}
void SetFileTimesTo(FILETIME* time, struct FileTimes* fileTimes) {
    fileTimes->creationTime = *time;
    fileTimes->lastAccessTime = *time;
    fileTimes->lastWriteTime = *time;
}
bool parseDateTime(const std::string& dateTimeStr, FILETIME& fileTime) {
    std::tm timeStruct = {};
    std::istringstream ss(dateTimeStr);

    ss >> std::get_time(&timeStruct, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        std::cerr << "Failed to parse date and time." << std::endl;
        return false;
    }

    std::time_t timeT = mktime(&timeStruct);

    if (timeT == -1) {
        std::cerr << "Failed to convert tm to time_t." << std::endl;
        return false;
    }

    ULARGE_INTEGER ull;
    ull.QuadPart = static_cast<unsigned __int64>(timeT) * 10000000LL + 116444736000000000ULL;

    fileTime.dwLowDateTime = ull.LowPart;
    fileTime.dwHighDateTime = ull.HighPart;

    return true;
}

void showHelp() {
    cout << "using：" << endl;
    cout << "  progname -f <target file path> -r <reference file path> [-h]" << endl;
    cout << "  -h help。" << endl;
}

bool parseArgs(int argc, char* argv[], string& path_f, string& path_r, string& path_d) {

    bool found_f = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc) {
                path_f = argv[i + 1];
                found_f = true;
            }
        }
        else if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 < argc) {
                path_r = argv[i + 1];
            }
        }
        else if (strcmp(argv[i], "-d") == 0) {
            if (i + 1 < argc) {
                path_d = argv[i + 1];
            }
        }
        else if (strcmp(argv[i], "-h") == 0) {
            showHelp();
            return false;
        }
    }

    return found_f;
}

bool setFileTime(const string& targetPath, const FileTimes& fileTimes) {
    HANDLE hFile = CreateFileA(targetPath.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "Unable to open the target file: " << GetLastError() << endl;
        return false;
    }
    if (!SetFileTime(hFile, &fileTimes.creationTime, &fileTimes.lastAccessTime, &fileTimes.lastWriteTime)) {
        cerr << "Failed to set file time: " << GetLastError() << endl;
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);
    return true;
}

FileTimes getFileTimes(const string& filePath) {
    HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "Unable to open reference file: " << GetLastError() << endl;
        exit(1);
    }

    FILETIME creationTime, lastAccessTime, lastWriteTime;
    if (!GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime)) {
        cerr << "Failed to get file time: " << GetLastError() << endl;
        CloseHandle(hFile);
        exit(1);
    }

    CloseHandle(hFile);
    return { creationTime, lastAccessTime, lastWriteTime };
}

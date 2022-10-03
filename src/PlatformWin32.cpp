
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <inttypes.h>

uint64_t Win32_GetLastWriteTime(const char* filePath) {
    uint64_t ret = 0;

    WIN32_FIND_DATA findData;
    HANDLE findHandle = FindFirstFile(filePath, &findData);
    if(findHandle != INVALID_HANDLE_VALUE) {
        ret = (uint64_t) findData.ftLastWriteTime.dwLowDateTime |
              (uint64_t) findData.ftLastWriteTime.dwHighDateTime << 32;
    }

    FindClose(findHandle);
    return ret;
}

bool Win32_FileHasChanged(FileData fileData) {
    uint64_t currentModifyTime = Win32_GetLastWriteTime(fileData.path.str);
    return fileData.changeTime < currentModifyTime;
}
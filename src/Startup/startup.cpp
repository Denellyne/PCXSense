#include "startup.h"
#include <filesystem>
#include <windows.h>
#include <iostream>
#include <fstream>
void setStartup(bool minimized) {
    extern std::string currentDirectory;

    int size = currentDirectory.length();
    HKEY hKey;
    LPCTSTR sk = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

    LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, sk, 0, KEY_ALL_ACCESS, &hKey);

    if (openRes == ERROR_SUCCESS)
        printf("Success opening key.");

    else
        printf("Error opening key.");

    std::ofstream autoStartScript("start.bat");
    if (autoStartScript.is_open()) {
        autoStartScript << std::format("cd \"{}\"",currentDirectory) << '\n';
        if(minimized)
            autoStartScript << "start PCXSense.exe minimized" << '\n';
        else
            autoStartScript << "start PCXSense.exe" << '\n';

        autoStartScript.close();
    }

    LPCSTR value = "PCXSense";

    LONG setRes = RegSetValueExA(hKey, value, 0, REG_SZ, (LPBYTE)(std::format("\"{}\\start.bat\"", currentDirectory).c_str()), size+16);

    if (setRes == ERROR_SUCCESS)
        printf("Success writing to Registry.");

    else
        printf("Error writing to Registry.");

    LONG closeOut = RegCloseKey(hKey);

    if (closeOut == ERROR_SUCCESS)
        printf("Success closing key.");

    else
        printf("Error closing key.");

}

bool startMinimized(const char* minimized) {
    if (minimized == nullptr) return 0;
    return !strcmp(minimized, "minimized");
}
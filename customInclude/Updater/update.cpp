#include "update.h"
#include <Urlmon.h>
#include <fstream>
#include <Windows.h>

void autoUpdater() {
    DownloadProgress downloadStatus;

    std::cout << "Checking if new update is avaiable\n";

    URLDownloadToFile(NULL, L"https://github.com/Denellyne/PCXSense/releases/download/Latest/Version.txt", L"Version.txt", 0, static_cast<IBindStatusCallback*>(&downloadStatus));
    downloadStatus.Release();

    std::ifstream checkVersion("Version.txt");
    char gitVersion[16];
    checkVersion.getline(gitVersion, 16);
    checkVersion.close();
    DeleteFile(L"Version.txt");

    if (!strcmp(gitVersion, Version.c_str())) {
        std::cout << "You're up to date\n";
    }
    else {
        ShellExecute(NULL, L"open", L"PCXSenseUpdater.exe", NULL, NULL, SW_SHOWDEFAULT);;
        _exit(NULL);
    }
}
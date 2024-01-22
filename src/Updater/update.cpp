#include "update.h"
#include <Urlmon.h>
#include <fstream>
#include <Windows.h>

#ifdef _DEBUG
void autoUpdater() {
    Sleep(3);
}
#else
void autoUpdater() {

    system("curl https://github.com/Denellyne/PCXSense/releases/download/Latest/Version.txt -o Version.txt -L");

    std::ifstream checkVersion("Version.txt");
    char gitVersion[16];
    checkVersion.getline(gitVersion, 16);
    checkVersion.close();
    DeleteFile(L"Version.txt");

    if (!strcmp(gitVersion, Version.c_str())) return;

    if (MessageBox(NULL, L"A new update was found do you wish to update?", L"New Update", MB_YESNO | MB_TASKMODAL) == IDNO) return;

    ShellExecute(0, 0, L"https://github.com/Denellyne/PCXSense/releases/tag/Latest", 0, 0, SW_SHOW);
    _exit(NULL);
}
#endif // _DEBUG

#pragma once

#include "Updater\update.h"
#include "User Settings/Macros/macro.h"
#include "User Settings/Game Profiles/gameProfile.h"

#define defaultWindowWidth 1280
#define defaultWindowHeigth 720

extern BOOL WINAPI exitFunction(_In_ DWORD dwCtrlType);

int GUI(controller& x360Controller, std::vector<Macros>& Macro, std::vector<gameProfile>& gameProfiles, bool minimized);


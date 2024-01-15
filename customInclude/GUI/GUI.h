#pragma once
#include "Misc\util.h"
#include "Updater\update.h"
#include "macro.h"

#define defaultWindowWidth 1280
#define defaultWindowHeigth 720

extern BOOL WINAPI exitFunction(_In_ DWORD dwCtrlType);

int GUI(controller& x360Controller, std::vector<Macros>& Macro);


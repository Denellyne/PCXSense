#pragma once
#include "Misc\util.h"
#include "Updater\update.h"

extern BOOL WINAPI exitFunction(_In_ DWORD dwCtrlType);

int GUI(controller& x360Controller, std::vector<Macros>& Macro);


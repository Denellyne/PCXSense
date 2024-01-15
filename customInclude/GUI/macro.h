#pragma once
#include "Misc/util.h"

void macroEditor(bool& makerOpen, Macros& macro, const controller& x360Controller);
void saveMacros(const std::vector<Macros> Macro);
void loadMacros(std::vector<Macros>& Macro);
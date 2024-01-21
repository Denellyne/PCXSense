#pragma once
#include "ControllerIO\controllerIO.h"

extern bool macroOpen;

struct Macros {
    std::string Name{};
    int buttonCombination{};
    INPUT input[2]{};

    Macros() {
        input[0].type = INPUT_KEYBOARD;
        input[1].type = INPUT_KEYBOARD;
    }
};

const char alphabet[26] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };

void macroEditor(bool& makerOpen, Macros& macro, const controller& x360Controller);
void saveMacros(const std::vector<Macros> Macro);
void loadMacros(std::vector<Macros>& Macro);
void asyncMacro(const controller& x360Controller, std::vector<Macros>& Macro);
void macroMenu(bool& profileMacroOpen, std::vector<Macros>& Macro, const controller& x360Controller);
void macroMenu(std::vector<Macros>& Macro, const controller& x360Controller);

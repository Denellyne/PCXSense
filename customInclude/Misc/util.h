#pragma once
#include <iostream>
#include "ControllerIO\controllerIO.h"
#include <vector>

#define DEBUG(x) do { std::cout << x << '\n'; } while (0)

struct Macros {
    std::string Name;
    int buttonCombination;
    INPUT input[2]{};
};

void saveMacros(std::vector<Macros>& Macro);
void asyncMacro(const controller& x360Controller, std::vector<Macros>& Macro);
void asyncDataReport(controller& inputReport);
int initializeFakeController(PVIGEM_TARGET & emulateX360, VIGEM_ERROR & target, PVIGEM_CLIENT & client);


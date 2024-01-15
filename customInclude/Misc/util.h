#pragma once
#include <iostream>
#include "ControllerIO\controllerIO.h"
#include <vector>

#define DEBUG(x) do { std::cout << x << '\n'; } while (0)



void asyncDataReport(controller& inputReport);
int initializeFakeController(PVIGEM_TARGET & emulateX360, VIGEM_ERROR & target, PVIGEM_CLIENT & client);


#pragma once
#include "ControllerIO\controllerIO.h"
#include <iostream>

#define DEBUG(x) do { std::cout << x << '\n'; } while (0)

void debugData(controller& inputReport);


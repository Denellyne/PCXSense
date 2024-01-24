#pragma once
#include "ControllerIO/controllerIO.h"

void lightbarEditor(bool& lightbarOpen, RGB* RGB);
void saveLightSettings(const controller& controller);
void loadLightSettings(controller& controller);
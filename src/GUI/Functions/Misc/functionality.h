#pragma once
#include "GUI\include.h"
#include "ControllerIO/controllerIO.h"
extern bool debugOpen;

bool loadTexture(GLuint* Images, GLFWwindow* window);
void setColors();
void rumleTestWindow(bool& rumbleWindow);
void debugMenu(controller& x360Controller);

#pragma once
#include "include.h"
#include "Misc/util.h"
extern bool debugOpen;

bool loadTexture(GLuint* Images, GLFWwindow* window);
void setColors();
void rumleTestWindow(bool& rumbleWindow);
void debugMenu(controller& x360Controller);

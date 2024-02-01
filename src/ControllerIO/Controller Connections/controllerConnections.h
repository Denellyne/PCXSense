#pragma once
#include "ControllerIO/controllerIO.h"

int initializeFakeController(PVIGEM_TARGET& emulateX360, VIGEM_ERROR& target, PVIGEM_CLIENT& client);
bool isDualsenseConnected(controller& x360Controller);

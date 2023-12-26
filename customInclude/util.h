#pragma once
#include <iostream>
#include <string>
#include <windows.h>
#include <dinput.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <hidapi.h>

#define DEBUG(x) do { std::cout << x << '\n'; } while (0)

void asyncDataReport(LPDIRECTINPUTDEVICE8& controllerInterface);
int initializeFakeController(HINSTANCE & appHandle, IDirectInput8 * ptrDirectInput, LPDIRECTINPUTDEVICE8 & controllerInterface, XINPUT_STATE & ControllerState, PVIGEM_TARGET & emulateX360, VIGEM_ERROR & target, PVIGEM_CLIENT & client);
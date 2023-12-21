#define DIRECTINPUT_VERSION 0x0800

#include <iostream>
#include <windows.h>
#include <DInput8.h>
#include <Xinput.h>

int main() {
	HINSTANCE appHandle = GetModuleHandle(NULL);
	IDirectInput8* ptrDirectInput = nullptr;

	if (DirectInput8Create(appHandle, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&ptrDirectInput, NULL) != DI_OK) {
		std::cout << "Failed to create DI8 Device\n";
		return -1;
	}
	LPDIRECTINPUTDEVICE8 controllerInterface;
	if (ptrDirectInput->CreateDevice(GUID_Joystick, &controllerInterface, NULL) != DI_OK) {
		ptrDirectInput->Release();
		std::cout << "Failed to create Device\n";
		return -1;
	}	
	if(controllerInterface->SetDataFormat(&c_dfDIJoystick2) != DI_OK){
		ptrDirectInput->Release();
		std::cout << "Failed to set Data Format\n";
		return -1;
	}
	if (controllerInterface->Acquire() != DI_OK) {
		ptrDirectInput->Release();
		std::cout << "Failed to acquire Device\n";
		return -1;
	}


	while (true) {
		
		HRESULT result = controllerInterface->Poll();
		DIJOYSTATE2 js{};	
		if (controllerInterface->GetDeviceState(sizeof(DIJOYSTATE2), &js) != DI_OK) {
			ptrDirectInput->Release();
			std::cout << "Failed to get Device State\n";
			return -1;
		}
		result = controllerInterface->GetDeviceState(sizeof(DIJOYSTATE2), &js);

		if (js.rgbButtons[0]) std::cout << "Square Button\n";
		if (js.rgbButtons[1]) std::cout << "X Button\n";
		if (js.rgbButtons[2]) std::cout << "Circle Button\n";
		if (js.rgbButtons[3]) std::cout << "Triangle Button\n";
		if (js.rgbButtons[4]) std::cout << "L1 Button\n";
		if (js.rgbButtons[5]) std::cout << "R1 Button\n";


	}

	ptrDirectInput->Release();

	return 0;
}
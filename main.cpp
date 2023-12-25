#define DIRECTINPUT_VERSION 0x0800

#include <iostream>
#include <string>
#include <windows.h>
#include <DInput8.h>
#include <Xinput.h>
#include "Include/ViGEm/Client.h"
#include <thread>

#define DEBUG(x) do { std::cout << x << '\n'; } while (0)

void asyncDataReport(LPDIRECTINPUTDEVICE8& controllerInterface) {

	while (true) {
		HRESULT result = controllerInterface->Poll();

		DIJOYSTATE2 joystick{};

		result = controllerInterface->GetDeviceState(sizeof(DIJOYSTATE2), &joystick);

		if (joystick.rgbButtons[0]) DEBUG("Square Button\n");

		if (joystick.rgbButtons[1]) DEBUG("X Button\n");

		if (joystick.rgbButtons[2]) DEBUG("Circle Button\n");

		if (joystick.rgbButtons[3]) DEBUG("Triangle Button\n");

		if (joystick.rgbButtons[4]) DEBUG("L1 Button\n");

		if (joystick.rgbButtons[5]) DEBUG("R1 Button\n");

		if (joystick.rgbButtons[8]) DEBUG("Select Button\n");

		if (joystick.rgbButtons[9]) DEBUG("Start Button\n");

		if (joystick.rgbButtons[10]) DEBUG("L3 Button\n");

		if (joystick.rgbButtons[11]) DEBUG("R3 Button\n");

		if (joystick.rgbButtons[12]) DEBUG("Sony/Home Button\n");

		if (joystick.rgbButtons[13]) DEBUG("Toutchpad Click\n");

		if (joystick.rgbButtons[14]) DEBUG("Microphone Button\n");

		if (controllerInterface->GetDeviceState(sizeof(DIJOYSTATE2), &joystick) == DI_OK) {
			std::cout << "LeftJoystick Horizontal Value: " << joystick.lX << '\n';
			std::cout << "LeftJoystick Vertical Value: " << joystick.lY << '\n';
			std::cout << "RightJoystick Horizontal Value: " << joystick.lZ << '\n';
			std::cout << "RightJoystick Horizontal Value: " << joystick.lRz << '\n';
			std::cout << "Left Trigger Value: " << joystick.lRx / 257 << '\n';
			std::cout << "Right Trigger Value: " << joystick.lRy / 257 << '\n';

			Sleep(1);
			system("cls"); //Clear console
		}

	}

}


int main() {

	//Initialize Fake Controller

	HINSTANCE appHandle = GetModuleHandle(NULL);
	IDirectInput8* ptrDirectInput = nullptr;
	LPDIRECTINPUTDEVICE8 controllerInterface;
	XINPUT_STATE ControllerState;

	const auto client = vigem_alloc();
	if (client == nullptr)
	{
		std::cerr << "Uh, not enough memory to do that?!" << std::endl;
		return -1;
	}

	const auto retval = vigem_connect(client);

	if (!VIGEM_SUCCESS(retval))
	{
		std::cerr << "ViGEm Bus connection failed with error code: 0x" << std::hex << retval << std::endl;
		return -1;
	}
	
	const auto emulateX360 = vigem_target_x360_alloc();

	const auto target = vigem_target_add(client, emulateX360);

	if (DirectInput8Create(appHandle, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&ptrDirectInput, NULL) != DI_OK) {
		std::cout << "Failed to create DI8 Device\n";
		return -1;
	}
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

	std::thread(asyncDataReport, std::ref(controllerInterface)).detach(); // Displays controller info

	while (true) {

		XInputGetState(0, &ControllerState);

		HWND foregroundAppHandle = GetForegroundWindow();

		HRESULT result = controllerInterface->Poll();

		DIJOYSTATE2 joystick{};	

		if (controllerInterface->GetDeviceState(sizeof(DIJOYSTATE2), &joystick) != DI_OK) { //Checks if controller is plugged in ,if not then tries to reconnect
			while (controllerInterface->Acquire() != DI_OK) {
				std::cout << "Failed to get Device State\n";
				std::cout << "Reconnecting";
				Sleep(500);
				std::cout << '.';
				Sleep(500);
				std::cout << '.';
				Sleep(500);
				std::cout << '.';
				Sleep(500);
				system("cls");
			}
		}

		result = controllerInterface->GetDeviceState(sizeof(DIJOYSTATE2), &joystick);

		ControllerState.Gamepad.wButtons = joystick.rgbButtons[0] ? XINPUT_GAMEPAD_X : 0;
		ControllerState.Gamepad.wButtons += joystick.rgbButtons[1] ? XINPUT_GAMEPAD_A : 0;
		ControllerState.Gamepad.wButtons += joystick.rgbButtons[2] ? XINPUT_GAMEPAD_B : 0;
		ControllerState.Gamepad.wButtons += joystick.rgbButtons[3] ? XINPUT_GAMEPAD_Y : 0;
		ControllerState.Gamepad.wButtons += joystick.rgbButtons[4] ? XINPUT_GAMEPAD_LEFT_SHOULDER : 0;
		ControllerState.Gamepad.wButtons += joystick.rgbButtons[5] ? XINPUT_GAMEPAD_RIGHT_SHOULDER : 0;		
		ControllerState.Gamepad.wButtons += joystick.rgbButtons[8] ? XINPUT_GAMEPAD_BACK : 0;
		ControllerState.Gamepad.wButtons += joystick.rgbButtons[9] ? XINPUT_GAMEPAD_START : 0;
		ControllerState.Gamepad.wButtons += joystick.rgbButtons[10] ? XINPUT_GAMEPAD_LEFT_THUMB : 0;
		ControllerState.Gamepad.wButtons += joystick.rgbButtons[11] ? XINPUT_GAMEPAD_RIGHT_THUMB : 0;
		//ControllerState.Gamepad.wButtons += joystick.rgbButtons[11] ? 0x20U << 8U : 0;

		switch (joystick.rgdwPOV[0]) {
		case 0:
			ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP;
			break;

		case 31500:
			ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_LEFT;
			break;

		case 4500: 
			ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_RIGHT;
			break;

		case 27000: 
			ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_LEFT;
			break;

		case 9000:  
			ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_RIGHT;
			break;

		case 18000:
			ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN;
			break;

		case 22500:
			ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_LEFT;
			break;

		case 13500:
			ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_RIGHT;
			break;			
		}

		ControllerState.Gamepad.bLeftTrigger = joystick.lRx/257;
		ControllerState.Gamepad.bRightTrigger = joystick.lRy / 257;
		ControllerState.Gamepad.sThumbLX= joystick.lX - 32768;
		ControllerState.Gamepad.sThumbLY =  32767 - joystick.lY;
		ControllerState.Gamepad.sThumbRX = joystick.lZ - 32768;
		ControllerState.Gamepad.sThumbRY = 32767 - joystick.lRz;

		vigem_target_x360_update(client, emulateX360, *reinterpret_cast<XUSB_REPORT*>(&ControllerState.Gamepad));
	}

	//Cleanup

	ptrDirectInput->Release();
	vigem_target_remove(client, emulateX360);
	vigem_target_free(emulateX360);
	vigem_disconnect(client);
	vigem_free(client);

	return 0;
}
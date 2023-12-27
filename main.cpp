#include "util.h"
#include <thread>
#define DS_VENDOR_ID 0x054c
#define DS_PRODUCT_ID 0x0ce6

VOID CALLBACK notification(
	PVIGEM_CLIENT Client,
	PVIGEM_TARGET Target,
	UCHAR LargeMotor,
	UCHAR SmallMotor,
	UCHAR LedNumber,
	LPVOID UserData
)
{
	hid_device_info* deviceInfo = hid_enumerate(DS_VENDOR_ID, DS_PRODUCT_ID);

	hid_device* dualsense = hid_open(DS_VENDOR_ID, DS_PRODUCT_ID, deviceInfo->serial_number);
	hid_free_enumeration(deviceInfo);

	unsigned char outputHID[48]{};
	outputHID[0] = 0x02;
	outputHID[1] = 0x02 | 0x01;
	outputHID[2] = 0x04;
	
	outputHID[45] = 90; //Red
	outputHID[46] = 0; //Green
	outputHID[47] = 229; //Blue

	outputHID[3] = SmallMotor; //Low Rumble
	outputHID[4] = LargeMotor; //High Rumble
	hid_write(dualsense, outputHID, 48);
	
}


int main() {

	//Initialize Fake Controller

	HINSTANCE appHandle = GetModuleHandle(NULL);
	IDirectInput8* ptrDirectInput = nullptr;
	LPDIRECTINPUTDEVICE8 controllerInterface;
	XINPUT_STATE ControllerState;
	PVIGEM_TARGET emulateX360;
	VIGEM_ERROR target;
	PVIGEM_CLIENT client = vigem_alloc();

	if (initializeFakeController(appHandle, ptrDirectInput, controllerInterface, ControllerState, emulateX360, target, client) != 0) return -1;

	std::thread(asyncDataReport, std::ref(controllerInterface)).detach(); // Displays controller info

	while (true) {

		XInputGetState(0, &ControllerState);

		HWND foregroundAppHandle = GetForegroundWindow();

		HRESULT result = controllerInterface->Poll();

		DIJOYSTATE2 joystick{};	

		if (controllerInterface->GetDeviceState(sizeof(DIJOYSTATE2), &joystick) != DI_OK)  //Checks if controller is plugged in ,if not then tries to reconnect
			while (controllerInterface->Acquire() != DI_OK) 


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

		vigem_target_x360_register_notification(client, emulateX360, &notification, nullptr);
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
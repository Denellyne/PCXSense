#include "Dualsense.h"

Dualsense::Dualsense() {
	client = vigem_alloc();

	if (client == NULL) {
		if (MessageBox(NULL, L"The app couldn't start, please install VigemBusDriver ,if this error persists please open an issue on github", L"Vigem bus", MB_YESNO | MB_TASKMODAL) == IDNO);
		ShellExecute(0, 0, L"https://github.com/nefarius/ViGEmBus/releases/tag/v1.22.0", 0, 0, SW_SHOW);
		_exit(NULL);
	}

	if (initializeFakeController(emulateX360, target, client) != 0) {
		if (MessageBox(NULL, L"The app couldn't start, please install VigemBusDriver ,if this error persists please open an issue on github", L"Vigem bus", MB_YESNO | MB_TASKMODAL) == IDNO);
		ShellExecute(0, 0, L"https://github.com/nefarius/ViGEmBus/releases/tag/v1.22.0", 0, 0, SW_SHOW);
		_exit(NULL);
	}
}

void inline Dualsense::setButtons(Controller& x360Controller) {

	// Normal Order
	x360Controller.ControllerState.Gamepad.wButtons = (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 4)) ? XINPUT_GAMEPAD_X : 0; //Square

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 5)) ? XINPUT_GAMEPAD_A : 0; //Cross

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 6)) ? XINPUT_GAMEPAD_B : 0; //Circle

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 7)) ? XINPUT_GAMEPAD_Y : 0; //Triangle

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 0)) ? XINPUT_GAMEPAD_LEFT_SHOULDER : 0; //Left Shoulder

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 1)) ? XINPUT_GAMEPAD_RIGHT_SHOULDER : 0; //Right Shoulder

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 4)) ? XINPUT_GAMEPAD_BACK : 0; //Select

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 5)) ? XINPUT_GAMEPAD_START : 0; //Start

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 6)) ? XINPUT_GAMEPAD_LEFT_THUMB : 0; //Left Thumb

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 7)) ? XINPUT_GAMEPAD_RIGHT_THUMB : 0; //Right thumb
	//ControllerState.Gamepad.wButtons += joystick.rgbButtons[12] ? 0x0400 : 0; //Sony Button

	switch ((int)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & 0x0f)) {
	case 0: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP; break;

	case 1: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_RIGHT; break;

	case 2: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_RIGHT; break;

	case 3: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_RIGHT; break;

	case 4: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN; break;

	case 5: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_LEFT; break;

	case 6: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_LEFT; break;

	case 7: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_LEFT; break;
	}
}

void inline setButtonsGameProfile(Controller& x360Controller) {

	extern int buttonMapping[11];

	// Normal Order
	x360Controller.ControllerState.Gamepad.wButtons = (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 4)) ? buttonMapping[0] : 0; //Square

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 5)) ? buttonMapping[1] : 0; //Cross

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 6)) ? buttonMapping[2] : 0; //Circle

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 7)) ? buttonMapping[3] : 0; //Triangle

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 0)) ? buttonMapping[4] : 0; //Left Shoulder

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 1)) ? buttonMapping[5] : 0; //Right Shoulder

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 4)) ? buttonMapping[6] : 0; //Select

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 5)) ? buttonMapping[7] : 0; //Start

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 6)) ? buttonMapping[8] : 0; //Left Thumb

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 7)) ? buttonMapping[9] : 0; //Right thumb
	//ControllerState.Gamepad.wButtons += joystick.rgbButtons[12] ? 0x0400 : 0; //Sony Button

	if (buttonMapping[10] == 1) {

		switch ((int)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & 0x0f)) {

		case 0: x360Controller.ControllerState.Gamepad.sThumbLY = 32767; break; //Up

		case 1: x360Controller.ControllerState.Gamepad.sThumbLY = 29727; x360Controller.ControllerState.Gamepad.sThumbLX = 19403; break; //Up Right

		case 2: x360Controller.ControllerState.Gamepad.sThumbLX = 32767; break; //Right

		case 3: x360Controller.ControllerState.Gamepad.sThumbLY = -29727; x360Controller.ControllerState.Gamepad.sThumbLX = 19403; break; //Down Right

		case 4: x360Controller.ControllerState.Gamepad.sThumbLY = -32768; break; //Down

		case 5: x360Controller.ControllerState.Gamepad.sThumbLY = -29727; x360Controller.ControllerState.Gamepad.sThumbLX = -22223; break; //Down Left

		case 6: x360Controller.ControllerState.Gamepad.sThumbLX = -32768; break; //Left

		case 7: x360Controller.ControllerState.Gamepad.sThumbLY = 29727; x360Controller.ControllerState.Gamepad.sThumbLX = -22223; break; //Up Left
		}
		return;
	}

	switch ((int)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & 0x0f)) {
	case 0: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP; break;

	case 1: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_RIGHT; break;

	case 2: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_RIGHT; break;

	case 3: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_RIGHT; break;

	case 4: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN; break;

	case 5: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_LEFT; break;

	case 6: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_LEFT; break;

	case 7: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_LEFT; break;
	}

}

void Dualsense::inputReport(Controller* x360Controller) {
	
	bool readSuccess = ReadFile(deviceHandle, inputBuffer, bufferSize, NULL, NULL);

	//if (!readSuccess) {
	//	CloseHandle(deviceHandle);
	//	//stop outputThread
//		while (!isControllerConnected(x360Controller)) {}
//		return;
//	}
	batteryLevel = (inputBuffer[53 + bluetooth] & 15) * 12.5; /* Hex 0x35 (USB) to get Battery / Hex 0x36 (Bluetooth) to get Battery
																											  because if bluetooth == true then bluetooth == 1 so we can just add bluetooth
																											  to the hex value of USB to get the battery reading
																										   */


																										   //Because of a bug on the Dualsense HID this needs to be implemented or else battery might display higher than 100 %
	batteryLevel = returnSmaller(batteryLevel, 100);

	ControllerState.Gamepad.sThumbLX = ((inputBuffer[1 + bluetooth] * 257) - 32768);
	ControllerState.Gamepad.sThumbLY = (32767 - (inputBuffer[2 + bluetooth] * 257));
	ControllerState.Gamepad.sThumbRX = ((inputBuffer[3 + bluetooth] * 257) - 32768);
	ControllerState.Gamepad.sThumbRY = (32767 - (inputBuffer[4 + bluetooth] * 257));

	//Same idea as boolSetter
	ControllerState.Gamepad.bLeftTrigger = inputBuffer[5 + bluetooth] * (shortTriggers == 0) + (((inputBuffer[5 + bluetooth]) >> 2) + 190) * (shortTriggers != 0);
	ControllerState.Gamepad.bRightTrigger = inputBuffer[6 + bluetooth] * (shortTriggers == 0) + (((inputBuffer[6 + bluetooth]) >> 2) + 190) * (shortTriggers != 0);

//	if (gameProfileSet) {
	//	setButtonsGameProfile((*x360Controller));
//		return;
	//}

	//setButtons((*x360Controller));

}
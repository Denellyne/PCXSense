#include "controllerIO.h"
#include <tlhelp32.h>
#include <tchar.h>
#include "User Settings/Adaptive Triggers/Adaptive Triggers.h"
#include "User Settings/Game Profiles/gameProfile.h"
#include <iostream>

float Red{ 210 }, Green{}, Blue{ 90 };
int AddRed{ 1 }, AddGreen{ 1 }, AddBlue{ 1 };
unsigned char outputHID[547]{};
constexpr DWORD TITLE_SIZE = 1024;
int emulator{};
extern bool gameProfileSet;


int initializeFakeController(PVIGEM_TARGET& emulateX360, VIGEM_ERROR& target, PVIGEM_CLIENT& client) {

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

	emulateX360 = vigem_target_x360_alloc();

	target = vigem_target_add(client, emulateX360);

	return 0;
}


bool inline isControllerConnected(controller& inputReport) {
	Sleep(50);
	hid_device_info* deviceInfo = hid_enumerate(DS_VENDOR_ID, DS_PRODUCT_ID);
	if (deviceInfo == nullptr) {
		inputReport.isConnected = false;
		return false;
	}
	inputReport.deviceHandle = CreateFileA(deviceInfo->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	inputReport.bluetooth = deviceInfo->interface_number == -1;

	if (inputReport.bluetooth) { //Bluetooth
		inputReport.bufferSize = 78;
		inputReport.inputBuffer[0] = 0x31; //Data report code
	}
	else { //USB
		inputReport.bufferSize = 64;
		inputReport.inputBuffer[0] = 0x01; //Data report code
	}
	hid_free_enumeration(deviceInfo);

	if ((bool)inputReport.deviceHandle) {
		inputReport.isConnected = true;
		return true;
	}
	else inputReport.isConnected = false;

	return false;
}

BOOL inline static CALLBACK FindWindowBySubstr(HWND hwnd, LPARAM substring){
	TCHAR windowTitle[TITLE_SIZE];

	if (GetWindowText(hwnd, windowTitle, TITLE_SIZE)) {
		if (_tcsstr(windowTitle, L"yuzu") != NULL) {
			
			emulator = 1;
			return false;
		}
			
		if (_tcsstr(windowTitle, L"Cemu") != NULL) {
			emulator = 2;
			return false;
		}

		if (_tcsstr(windowTitle, L"Dolphin") != NULL) {
			emulator = 3;
			return false;
		}
	}

	emulator = 0;
	return true;
}

void inline adaptiveTriggersProfile(bool& bluetooth, int& shortTriggers);

void extern inline sendOutputReport(controller& x360Controller) {
	
	while (true) {
		Sleep(4);
		ZeroMemory(outputHID, 547);

		outputHID[3 + x360Controller.bluetooth] = rumble[0]; //Low Rumble
		outputHID[4 + x360Controller.bluetooth] = rumble[1]; //High Rumble

		outputHID[9 + x360Controller.bluetooth] = 0x00; //LED Controls
		outputHID[39 + x360Controller.bluetooth] = 0x02;
		outputHID[42 + x360Controller.bluetooth] = 0x02;
		outputHID[43 + x360Controller.bluetooth] = 0x02;

		adaptiveTriggersProfile(x360Controller.bluetooth, x360Controller.shortTriggers);

		if (x360Controller.bluetooth) {
			outputHID[0] = 0x31;
			outputHID[1] = 0x02;
			outputHID[2] = 0x03 | 0x04 | 0x08;
			outputHID[3] = 0x55;

			switch (x360Controller.batteryLevel) {
			case 0:
				outputHID[10] = 0x02; // Microphone Button Pulsating
				outputHID[46] = 255; //Red
				break;
			case 12:
				outputHID[10] = 0x01; // Microphone button On
				outputHID[46] = 200; //Red
				outputHID[47] = 0; //Green
				outputHID[48] = 55; //Blue
				break;
			case 27:
				outputHID[46] = 230; //Red
				outputHID[47] = 90; //Green
				outputHID[48] = 0; //Blue
				break;
			case 37:
				outputHID[46] = 230; //Red
				outputHID[47] = 130; //Green
				outputHID[48] = 0; //Blue
				break;
			case 50:
				outputHID[46] = 140; //Red
				outputHID[47] = 30; //Green
				outputHID[48] = 90; //Blue
				break;
			case 67:
				outputHID[46] = 90; //Red
				outputHID[47] = 0; //Green
				outputHID[48] = 140; //Blue
				break;
			case 75:
				outputHID[46] = 90; //Red
				outputHID[47] = 0; //Green
				outputHID[48] = 80; //Blue
				break;
			case 87:
				outputHID[46] = 110; //Red
				outputHID[47] = 0; //Green
				outputHID[48] = 80; //Blue
				break;
			case 100:
				outputHID[46] = 200; //Red
				outputHID[47] = 0; //Green
				outputHID[48] = 229; //Blue
				break;
			default:
				outputHID[46] = 255; //Red
				outputHID[47] = 140; //Green
				outputHID[48] = 0; //Blue
				break;
			}

			const UINT32 crc = computeCRC32(outputHID, 74);

			outputHID[74] = (crc & 0x000000FF);
			outputHID[75] = ((crc & 0x0000FF00) >> 8UL);
			outputHID[76] = ((crc & 0x00FF0000) >> 16UL);
			outputHID[77] = ((crc & 0xFF000000) >> 24UL);

			WriteFile(x360Controller.deviceHandle, outputHID, 547, NULL, NULL);

		}
		else {
			//USB
			outputHID[0] = 0x02;
			outputHID[1] = 0x03 | 0x04 | 0x08;
			outputHID[2] = 0x55;

			switch (x360Controller.batteryLevel) {
			case 75:
				outputHID[45] = 20; //Red
				outputHID[46] = 170; //Green
				outputHID[47] = 150; //Blue
				break;
			case 87:
				outputHID[45] = 20; //Red
				outputHID[46] = 170; //Green
				outputHID[47] = 150; //Blue
				break;
			case 100:
				outputHID[45] = 130; //Red
				outputHID[46] = 30; //Green
				outputHID[47] = 255; //Blue
				break;
			default:
				outputHID[45] = 255; //Red
				outputHID[46] = 140; //Green
				outputHID[47] = 0; //Blue
				break;
			}

			WriteFile(x360Controller.deviceHandle, outputHID, 64, NULL, NULL);
		}

		x360Controller.RGB[0].red = outputHID[45 + x360Controller.bluetooth];
		x360Controller.RGB[0].green = outputHID[46 + x360Controller.bluetooth];
		x360Controller.RGB[0].blue = outputHID[47 + x360Controller.bluetooth];
	}

	/*if (x360Controller.rainbow) {
				if (Red == 255) AddRed = -1;
				if (Red == 0) AddRed = 1;
				if (Green == 255) AddGreen = -1;
				if (Green == 0) AddGreen = 1;
				if (Blue == 255) AddBlue = -1;
				if (Blue == 0) AddBlue = 1;

				Red += 2.5f * AddRed;
				Green += 2.5f * AddGreen;
				Blue += 2.5f * AddBlue;

				outputHID[45] = Red; //Red
				outputHID[46] = Green; //Green
				outputHID[47] = Blue; //Blue
			}*/

}

void inline getInputReport(controller& x360Controller) {

	//static uint8_t isCharging = (x360Controller.inputBuffer[53 + x360Controller.bluetooth] & 0xf0) >> 0x4;

	bool readSuccess = ReadFile(x360Controller.deviceHandle, x360Controller.inputBuffer, x360Controller.bufferSize, NULL, NULL);

	if (!readSuccess) {
		CloseHandle(x360Controller.deviceHandle);
		while (!isControllerConnected(x360Controller)) {}
	}

	x360Controller.batteryLevel = (x360Controller.inputBuffer[53 + x360Controller.bluetooth] & 15) * 12.5; /* Hex 0x35 (USB) to get Battery / Hex 0x36 (Bluetooth) to get Battery
																									 because if bluetooth == true then bluetooth == 1 so we can just add bluetooth
																									 to the hex value of USB to get the battery reading
																									 */

	x360Controller.batteryLevel = x360Controller.batteryLevel > 100 ? 100 : x360Controller.batteryLevel; //Because of a bug on the HID this needs to be implemented or else battery might display higher than 100%

	if (bool(x360Controller.shortTriggers)) {
		x360Controller.ControllerState.Gamepad.bLeftTrigger = (x360Controller.inputBuffer[5 + x360Controller.bluetooth]) * 0.25 + 190;
		x360Controller.ControllerState.Gamepad.bRightTrigger = (x360Controller.inputBuffer[6 + x360Controller.bluetooth]) * 0.25 + 190;
	}
	else {
		x360Controller.ControllerState.Gamepad.bLeftTrigger = x360Controller.inputBuffer[5 + x360Controller.bluetooth];
		x360Controller.ControllerState.Gamepad.bRightTrigger = x360Controller.inputBuffer[6 + x360Controller.bluetooth];
	}


	x360Controller.ControllerState.Gamepad.sThumbLX = ((x360Controller.inputBuffer[1 + x360Controller.bluetooth] * 257) - 32768);
	x360Controller.ControllerState.Gamepad.sThumbLY = (32767 - (x360Controller.inputBuffer[2 + x360Controller.bluetooth] * 257));
	x360Controller.ControllerState.Gamepad.sThumbRX = ((x360Controller.inputBuffer[3 + x360Controller.bluetooth] * 257) - 32768);
	x360Controller.ControllerState.Gamepad.sThumbRY = (32767 - (x360Controller.inputBuffer[4 + x360Controller.bluetooth] * 257));

	x360Controller.ControllerState.Gamepad.wButtons = (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 4)) ? XINPUT_GAMEPAD_X : 0;

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 5)) ? XINPUT_GAMEPAD_A : 0;

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 6)) ? XINPUT_GAMEPAD_B : 0;

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 7)) ? XINPUT_GAMEPAD_Y : 0;

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 0)) ? XINPUT_GAMEPAD_LEFT_SHOULDER : 0;

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 1)) ? XINPUT_GAMEPAD_RIGHT_SHOULDER : 0;

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 4)) ? XINPUT_GAMEPAD_BACK : 0;

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 5)) ? XINPUT_GAMEPAD_START : 0;

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 6)) ? XINPUT_GAMEPAD_LEFT_THUMB : 0;

	x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 7)) ? XINPUT_GAMEPAD_RIGHT_THUMB : 0;
	//ControllerState.Gamepad.wButtons += joystick.rgbButtons[12] ? 0x0400 : 0;

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

void inline adaptiveTriggersProfile(bool& bluetooth, int& shortTriggers) {
		EnumWindows(FindWindowBySubstr, NULL);

		switch (emulator) {
		case 1: //Yuzu
			shortTriggers = 190;
			outputHID[11 + bluetooth] = 0x2;
			outputHID[12 + bluetooth] = 30;
			outputHID[13 + bluetooth] = 180;
			outputHID[14 + bluetooth] = 50;

			outputHID[22 + bluetooth] = 0x2;
			outputHID[23 + bluetooth] = 23;
			outputHID[24 + bluetooth] = 180;
			outputHID[25 + bluetooth] = 50;

			break;
		case 2: // Cemu
			shortTriggers = 0;
			outputHID[11 + bluetooth] = 0x2;
			outputHID[12 + bluetooth] = 30;
			outputHID[13 + bluetooth] = 180;
			outputHID[14 + bluetooth] = 50;

			outputHID[22 + bluetooth] = 0x2;
			outputHID[23 + bluetooth] = 23;
			outputHID[24 + bluetooth] = 180;
			outputHID[25 + bluetooth] = 50;

			break;
		case 3: //Dolphin

			shortTriggers = 0;
			outputHID[11 + bluetooth] = 0x2;
			outputHID[12 + bluetooth] = 0x90;
			outputHID[13 + bluetooth] = 0xA0;
			outputHID[14 + bluetooth] = 0xFF;

			outputHID[22 + bluetooth] = 0x2;
			outputHID[23 + bluetooth] = 0x90;
			outputHID[24 + bluetooth] = 0xA0;
			outputHID[25 + bluetooth] = 0xFF;

			break;
		default:
			//Else set Current Trigger Profile
			shortTriggers = 0;
			if (!gameProfileSet) break;

			memcpy(&outputHID[11 + bluetooth], &ptrCurrentTriggerProfile, 7);
			memcpy(&outputHID[22 + bluetooth], &ptrCurrentTriggerProfile, 7);
			outputHID[20 + bluetooth] = ptrCurrentTriggerProfile[7];
			outputHID[31 + bluetooth] = ptrCurrentTriggerProfile[7];
			break;
		}

}

uint32_t inline computeCRC32(unsigned char* buffer, const size_t& len)
{
	UINT32 result = crcSeed;
	for (size_t i = 0; i < len; i++)
		// Compute crc
		result = hashTable[((unsigned char)result) ^ ((unsigned char)buffer[i])] ^ (result >> 8);
	// Return result
	return result;
}
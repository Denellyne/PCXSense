#include "controllerIO.h"
#include <tlhelp32.h>
#include <tchar.h>

BOOL CALLBACK FindWindowBySubstr(HWND hwnd, LPARAM substring)
{
	const DWORD TITLE_SIZE = 1024;
	TCHAR windowTitle[TITLE_SIZE];

	if (GetWindowText(hwnd, windowTitle, TITLE_SIZE))
		if (_tcsstr(windowTitle, LPCTSTR(substring)) != NULL)
			return false;

	return true;
}

void inline isEmulatorRunning(unsigned char* outputHID, int bluetooth, int& shortTriggers);

void extern inline sendOutputReport(controller& x360Controller) {
	float Red{ 210 }, Green{}, Blue{ 90 };
	bool AddRed{ true }, AddGreen{ true }, AddBlue{ true };
	unsigned char outputHID[547]{};

	while (true) {
		Sleep(4);
		if (x360Controller.bluetooth) {
			ZeroMemory(outputHID, 547);


			outputHID[0] = 0x31;
			outputHID[1] = 0x02;
			outputHID[2] = 0x03 | 0x04 | 0x08;
			outputHID[3] = 0x55;

			outputHID[4] = rumble[0]; //Low Rumble
			outputHID[5] = rumble[1]; //High Rumble

			outputHID[10] = 0x00;
			outputHID[40] = 0x02;
			outputHID[43] = 0x02;
			outputHID[44] = 0x02;

			switch (x360Controller.batteryLevel) {
			case 0:
				outputHID[10] = 0x02;
				outputHID[46] = 255; //Red
				break;
			case 12:
				outputHID[10] = 0x01;
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

			isEmulatorRunning(outputHID, x360Controller.bluetooth, x360Controller.shortTriggers);

			if (x360Controller.rainbow) {
				if (Red == 255) AddRed = false;
				if (Red == 0) AddRed = true;
				if (Green == 255) AddGreen = false;
				if (Green == 0) AddGreen = true;
				if (Blue == 255) AddBlue = false;
				if (Blue == 0) AddBlue = true;

				if (AddRed) Red+=2.5f;
				else Red -= 2.5f;
				if (AddGreen) Green += 5;
				else Green -= 5;
				if (AddBlue) Blue += 2.5f;
				else Blue -= 2.5f;

				outputHID[46] = Red; //Red
				outputHID[47] = Green; //Green
				outputHID[48] = Blue; //Blue
			}

			const UINT32 crc = computeCRC32(outputHID, 74);

			outputHID[74] = (crc & 0x000000FF);
			outputHID[75] = ((crc & 0x0000FF00) >> 8UL);
			outputHID[76] = ((crc & 0x00FF0000) >> 16UL);
			outputHID[77] = ((crc & 0xFF000000) >> 24UL);

			x360Controller.RGB.red = outputHID[46];
			x360Controller.RGB.green = outputHID[47];
			x360Controller.RGB.blue = outputHID[48];

			WriteFile(x360Controller.deviceHandle, outputHID, 547, NULL, NULL);
		}
		else {
			ZeroMemory(outputHID, 547);

			outputHID[0] = 0x02;
			outputHID[1] = 0x03 | 0x04 | 0x08;
			outputHID[2] = 0x55;

			outputHID[3] = rumble[0]; //Low Rumble
			outputHID[4] = rumble[1]; //High Rumble

			outputHID[9] = 0x00; //LED Controls
			outputHID[39] = 0x02;
			outputHID[42] = 0x02;
			outputHID[43] = 0x02;

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

			isEmulatorRunning(outputHID, x360Controller.bluetooth, x360Controller.shortTriggers);

			if (x360Controller.rainbow) {
				if (Red == 255) AddRed = false;
				if (Red == 0) AddRed = true;
				if (Green == 255) AddGreen = false;
				if (Green == 0) AddGreen = true;
				if (Blue == 255) AddBlue = false;
				if (Blue == 0) AddBlue = true;

				if (AddRed) Red += 2.5f;
				else Red -= 2.5f;
				if (AddGreen) Green += 5;
				else Green -= 5;
				if (AddBlue) Blue += 2.5f;
				else Blue -= 2.5f;

				outputHID[45] = Red; //Red
				outputHID[46] = Green; //Green
				outputHID[47] = Blue; //Blue
			}
			x360Controller.RGB.red = outputHID[45];
			x360Controller.RGB.green = outputHID[46];
			x360Controller.RGB.blue = outputHID[47];

			WriteFile(x360Controller.deviceHandle, outputHID, 64, NULL, NULL);
		}
	}

}

void inline getInputReport(controller& x360Controller) {

	static uint8_t isCharging = (x360Controller.inputBuffer[53 + x360Controller.bluetooth] & 0xf0) >> 0x4;

	bool readSuccess = ReadFile(x360Controller.deviceHandle, x360Controller.inputBuffer, x360Controller.bufferSize, NULL, NULL);

	x360Controller.batteryLevel = (x360Controller.inputBuffer[53 + x360Controller.bluetooth] & 15) * 12.5; /* Hex 0x35 (USB) to get Battery / Hex 0x36 (Bluetooth) to get Battery
																									 because if bluetooth == true then bluetooth == 1 so we can just add bluetooth
																									 to the hex value of USB to get the battery reading
																									 */


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

	if (!readSuccess) {
		CloseHandle(x360Controller.deviceHandle);
		while (!isControllerConnected(x360Controller)) {}
	}
}

void inline isEmulatorRunning(unsigned char* outputHID, int bluetooth, int& shortTriggers) {
	if (EnumWindows(FindWindowBySubstr, (LPARAM)L"yuzu") == false) {

		shortTriggers = 190;
		outputHID[11 + bluetooth] = 0x2;
		outputHID[12 + bluetooth] = 30;
		outputHID[13 + bluetooth] = 180;
		outputHID[14 + bluetooth] = 50;

		outputHID[22 + bluetooth] = 0x2;
		outputHID[23 + bluetooth] = 23;
		outputHID[24 + bluetooth] = 180;
		outputHID[25 + bluetooth] = 50;

		return;
	}

	if (EnumWindows(FindWindowBySubstr, (LPARAM)L"Cemu") == false) {

		shortTriggers = 0;
		outputHID[11 + bluetooth] = 0x2;
		outputHID[12 + bluetooth] = 30;
		outputHID[13 + bluetooth] = 180;
		outputHID[14 + bluetooth] = 50;

		outputHID[22 + bluetooth] = 0x2;
		outputHID[23 + bluetooth] = 23;
		outputHID[24 + bluetooth] = 180;
		outputHID[25 + bluetooth] = 50;

		return;
	}

	if (EnumWindows(FindWindowBySubstr, (LPARAM)L"Dolphin ") == false) {

		shortTriggers = 0;
		outputHID[11 + bluetooth] = 0x2; //Mode Motor Right
		outputHID[12 + bluetooth] = 0x90; //right trigger start of resistance section
		outputHID[13 + bluetooth] = 0xA0; //right trigger (mode1) amount of force exerted (mode2) end of resistance section supplemental mode 4+20) flag(s?) 0x02 = do not pause effect when fully presse
		outputHID[14 + bluetooth] = 0xFF; //right trigger force exerted in range (mode2)
		//	outputHID[15 + bluetooth] = 0x0; // strength of effect near release state (requires supplement modes 4 and 20)
			//outputHID[16 + bluetooth] = 0x0; // strength of effect near middle (requires supplement modes 4 and 20)
		//	outputHID[17 + bluetooth] = 0x0; // strength of effect at pressed state (requires supplement modes 4 and 20)
	//		outputHID[20 + bluetooth] = 0x0; // effect actuation frequency in Hz (requires supplement modes 4 and 20)

		outputHID[22 + bluetooth] = 0x2; //Mode Motor Left
		outputHID[23 + bluetooth] = 0x90; //Left trigger start of resistance section
		outputHID[24 + bluetooth] = 0xA0; //Left trigger (mode1) amount of force exerted (mode2) end of resistance section supplemental mode 4+20) flag(s?) 0x02 = do not pause effect when fully presse
		outputHID[25 + bluetooth] = 0xFF; //Left trigger force exerted in range (mode2)
		//	outputHID[26 + bluetooth] = 0x0; // strength of effect near release state (requires supplement modes 4 and 20)
		//	outputHID[27 + bluetooth] = 0x0; // strength of effect near middle (requires supplement modes 4 and 20)
		//	outputHID[28 + bluetooth] = 0x0; // strength of effect at pressed state (requires supplement modes 4 and 20)
		//	outputHID[31 + bluetooth] = 0x0; // effect actuation frequency in Hz (requires supplement modes 4 and 20)
		return;
	}

	

	ZeroMemory(outputHID, 0);
	shortTriggers = 0;
}


uint32_t computeCRC32(unsigned char* buffer, const size_t& len)
{
	UINT32 result = crcSeed;
	for (size_t i = 0; i < len; i++)
		// Compute crc
		result = hashTable[((unsigned char)result) ^ ((unsigned char)buffer[i])] ^ (result >> 8);

	// Return result
	return result;
}
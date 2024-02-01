#include "controllerIO.h"
#include <tlhelp32.h>
#include <tchar.h>
#include "User Settings/Adaptive Triggers/Adaptive Triggers.h"
#include "User Settings/Game Profiles/gameProfile.h"
#include "Controller Connections/controllerConnections.h"
#include "thread"
#if (defined _DEBUG || defined _PROFILE)
#include "Misc/benchmark.h"
#else
#define DEBUG(x) do ; while(0)
#endif // _DEBUG

unsigned char outputHID[547]{};
constexpr DWORD TITLE_SIZE = 1024;
int emulator{};
extern bool gameProfileSet;

//#define boolSetter(x,y,operatorA,operatorB) x * (x operatorA y) + y * (y operatorB x);

extern "C" int returnSmaller(int x, int y);

bool inline isControllerConnected(controller& x360Controller) {
	Sleep(50); //Sleeps for 50ms so it doesnt consume to much memory and CPU

	//Stop output thread
	extern LPVOID asyncThreadPointer;
	if (reinterpret_cast<std::thread*>(asyncThreadPointer) != nullptr) {
		reinterpret_cast<std::thread*>(asyncThreadPointer)->~thread();
		delete asyncThreadPointer;
		asyncThreadPointer = nullptr;
	}
		
	extern void (*getInputs)(controller& x360Controller);

	if (isDualsenseConnected(x360Controller)) {
		getInputs = &getDualsenseInput;
		std::thread* asyncThreadPointer = new std::thread(sendDualsenseOutputReport, std::ref(x360Controller));
		asyncThreadPointer->detach();
		return true;
	}

	x360Controller.isConnected = false;
	return false;

}

BOOL inline static CALLBACK FindWindowBySubstr(HWND hwnd, LPARAM substring){
	TCHAR windowTitle[TITLE_SIZE];

	if (GetWindowText(hwnd, windowTitle, TITLE_SIZE)) {
		if (_tcsstr(windowTitle, LPCTSTR(substring)) != NULL)
			return true;
	}

	return false;
}

void inline adaptiveTriggersProfile(bool& bluetooth, int& shortTriggers);

void extern inline sendDualsenseOutputReport(controller& x360Controller) {
	extern bool profileOpen;
	extern bool lightbarOpen;
	extern bool profileEdit;

	while (true) {
		Sleep(4);


		ZeroMemory(outputHID, 547);

		//USB Report ID or BT additional Flag
		outputHID[0 + x360Controller.bluetooth] = 0x02;

		//Trigger Flags
		outputHID[1 + x360Controller.bluetooth] = 0x03 | 0x04 | 0x08;
		outputHID[2 + x360Controller.bluetooth] = 0x55;

		outputHID[3 + x360Controller.bluetooth] = rumble[0]; //Low Rumble
		outputHID[4 + x360Controller.bluetooth] = rumble[1]; //High Rumble

		adaptiveTriggersProfile(x360Controller.bluetooth, x360Controller.shortTriggers);

		switch (x360Controller.batteryLevel + lightbarOpen + gameProfileSet + profileEdit) { //Chooses Lightbar Profile, if any of those editors are open then it breaks and sets itself to the currently being edited profile
		case 0: x360Controller.RGB[0].Index = 1; break;
		case 12: x360Controller.RGB[0].Index = 2; break;
		case 25: x360Controller.RGB[0].Index = 3; break;
		case 37: x360Controller.RGB[0].Index = 4; break;
		case 50: x360Controller.RGB[0].Index = 5; break;
		case 62: x360Controller.RGB[0].Index = 6; break;
		case 75: x360Controller.RGB[0].Index = 7; break;
		case 87: x360Controller.RGB[0].Index = 8; break;
		case 100: x360Controller.RGB[0].Index = 9; break;
		default: break;
		}

LightEditorOpened:

		outputHID[9 + x360Controller.bluetooth] = x360Controller.RGB[x360Controller.RGB[0].Index].microhponeLed;
		outputHID[39 + x360Controller.bluetooth] = 0x02;
		outputHID[42 + x360Controller.bluetooth] = 0x02;
		outputHID[43 + x360Controller.bluetooth] = 0x02;


		for (int i = 0; i < 3; i++) {
			outputHID[45 + x360Controller.bluetooth + i] = x360Controller.RGB[x360Controller.RGB[0].Index].colors[i] * 255;
			x360Controller.RGB[0].colors[i] = x360Controller.RGB[x360Controller.RGB[0].Index].colors[i];
		}

		//Send Output Report

		if (x360Controller.bluetooth) {
			outputHID[0] = 0x31; // BT Report ID
			
			const UINT32 crc = computeCRC32(outputHID, 74);

			outputHID[74] = (crc & 0x000000FF);
			outputHID[75] = ((crc & 0x0000FF00) >> 8UL);
			outputHID[76] = ((crc & 0x00FF0000) >> 16UL);
			outputHID[77] = ((crc & 0xFF000000) >> 24UL);

			WriteFile(x360Controller.deviceHandle, outputHID, 547, NULL, NULL);
			continue;
		}
		//USB
		WriteFile(x360Controller.deviceHandle, outputHID, 64, NULL, NULL);
			
	}

}

void inline static setButtons(controller& x360Controller){

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

void inline static setButtonsGameProfile(controller& x360Controller) {

	extern int buttonMapping[11];

	// Normal Order
	x360Controller.ControllerState.Gamepad.wButtons = (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 4))  ? buttonMapping[0] : 0; //Square

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

void inline getDualsenseInput(controller& x360Controller) {

	bool readSuccess = ReadFile(x360Controller.deviceHandle, x360Controller.inputBuffer, x360Controller.bufferSize, NULL, NULL);

	if (!readSuccess) {
		CloseHandle(x360Controller.deviceHandle);
		while (!isControllerConnected(x360Controller)) {}
		return;
	}
	x360Controller.batteryLevel = (x360Controller.inputBuffer[53 + x360Controller.bluetooth] & 15) * 12.5; /* Hex 0x35 (USB) to get Battery / Hex 0x36 (Bluetooth) to get Battery
																											  because if bluetooth == true then bluetooth == 1 so we can just add bluetooth
																										  	  to the hex value of USB to get the battery reading
															 											   */


	//Because of a bug on the Dualsense HID this needs to be implemented or else battery might display higher than 100 %
	x360Controller.batteryLevel = returnSmaller(x360Controller.batteryLevel, 100);

	x360Controller.ControllerState.Gamepad.sThumbLX = ((x360Controller.inputBuffer[1 + x360Controller.bluetooth] * 257) - 32768);
	x360Controller.ControllerState.Gamepad.sThumbLY = (32767 - (x360Controller.inputBuffer[2 + x360Controller.bluetooth] * 257));
	x360Controller.ControllerState.Gamepad.sThumbRX = ((x360Controller.inputBuffer[3 + x360Controller.bluetooth] * 257) - 32768);
	x360Controller.ControllerState.Gamepad.sThumbRY = (32767 - (x360Controller.inputBuffer[4 + x360Controller.bluetooth] * 257));

	//Same idea as boolSetter
	x360Controller.ControllerState.Gamepad.bLeftTrigger = x360Controller.inputBuffer[5 + x360Controller.bluetooth] * (x360Controller.shortTriggers == 0) + (((x360Controller.inputBuffer[5 + x360Controller.bluetooth]) >> 2) + 190) * (x360Controller.shortTriggers != 0);
	x360Controller.ControllerState.Gamepad.bRightTrigger = x360Controller.inputBuffer[6 + x360Controller.bluetooth] * (x360Controller.shortTriggers == 0) + (((x360Controller.inputBuffer[6 + x360Controller.bluetooth]) >> 2) + 190) * (x360Controller.shortTriggers != 0);

	if (gameProfileSet) {
		setButtonsGameProfile(x360Controller);
		return;
	}

	setButtons(x360Controller);

} 

void inline adaptiveTriggersProfile(bool& bluetooth, int& shortTriggers) {
	HWND foregroundWindow = GetForegroundWindow();

	emulator = 1 * (FindWindowBySubstr(foregroundWindow, (LPARAM)L"yuzu")) + 2 * (FindWindowBySubstr(foregroundWindow, (LPARAM)L"Cemu")) + 3 * (FindWindowBySubstr(foregroundWindow, (LPARAM)L"Dolphin"));

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


//Useful data for later

/*
* Benchmark template
#if (defined _DEBUG || defined _PROFILE)
	Timer inputLagInMicroSeconds(4);
#endif
*/

//static uint8_t isCharging = (x360Controller.inputBuffer[53 + x360Controller.bluetooth] & 0xf0) >> 0x4;

/*if (x360Controller.rainbow) {
*
* //static float Red{ 210 }, Green{}, Blue{ 90 };
  //static int AddRed{ 1 }, AddGreen{ 1 }, AddBlue{ 1 };
*
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

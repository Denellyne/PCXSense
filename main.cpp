#include "util.h"

#include <thread>
#define DS_VENDOR_ID 0x054c
#define DS_PRODUCT_ID 0x0ce6

//typedef DWORD(WINAPI* XInputGetStateExProc)(DWORD dwUserIndex, XINPUT_STATE* pState);

	//HMODULE xinput_lib = LoadLibraryA("xinput1_3.dll");
	//XInputGetStateExProc XInputGetStateEx{};
	//int XInputGetStateExOrdinal = 100;
//	XInputGetStateEx = (XInputGetStateExProc)GetProcAddress(xinput_lib, (LPCSTR)XInputGetStateExOrdinal);
	
extern UCHAR rumble[2]{};


VOID CALLBACK getRumble(
	PVIGEM_CLIENT Client,
	PVIGEM_TARGET Target,
	UCHAR LargeMotor,
	UCHAR SmallMotor,
	UCHAR LedNumber,
	LPVOID UserData
)
{
	rumble[0] = SmallMotor;
	rumble[1] = LargeMotor;
}


int main() {

	//Initialize Fake Controller

	XINPUT_STATE ControllerState;
	PVIGEM_TARGET emulateX360;
	VIGEM_ERROR target;
	PVIGEM_CLIENT client = vigem_alloc();

	inputReport inputReport{};

	std::thread(asyncGetInputReport, std::ref(inputReport)).detach(); //Gets the Controllers Input through the HID Input Report
	std::thread(asyncSendOutputReport, std::ref(inputReport)).detach();

	std::thread(asyncDataReport,std::ref(inputReport)).detach(); // Displays controller info

	if (initializeFakeController(ControllerState, emulateX360, target, client) != 0) return -1;
	vigem_target_x360_register_notification(client, emulateX360, &getRumble, nullptr);

	while (true) {
		XInputGetState(0, &ControllerState);

		ControllerState.Gamepad.wButtons =  (bool)(inputReport.inputBuffer[8 + inputReport.bluetooth]  & (1 << 4)) ? XINPUT_GAMEPAD_X : 0;

		ControllerState.Gamepad.wButtons += (bool)(inputReport.inputBuffer[8 + inputReport.bluetooth]  & (1 << 5)) ? XINPUT_GAMEPAD_A : 0;

		ControllerState.Gamepad.wButtons += (bool)(inputReport.inputBuffer[8 + inputReport.bluetooth]  & (1 << 6)) ? XINPUT_GAMEPAD_B : 0;

		ControllerState.Gamepad.wButtons += (bool)(inputReport.inputBuffer[8 + inputReport.bluetooth]  & (1 << 7)) ? XINPUT_GAMEPAD_Y : 0;

		ControllerState.Gamepad.wButtons += (bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 0)) ? XINPUT_GAMEPAD_LEFT_SHOULDER : 0;

		ControllerState.Gamepad.wButtons += (bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 1)) ? XINPUT_GAMEPAD_RIGHT_SHOULDER : 0;

		ControllerState.Gamepad.wButtons += (bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 4)) ? XINPUT_GAMEPAD_BACK : 0;

		ControllerState.Gamepad.wButtons += (bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 5)) ? XINPUT_GAMEPAD_START : 0;

		ControllerState.Gamepad.wButtons += (bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 6)) ? XINPUT_GAMEPAD_LEFT_THUMB : 0;

		ControllerState.Gamepad.wButtons += (bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 7)) ? XINPUT_GAMEPAD_RIGHT_THUMB : 0;
		//ControllerState.Gamepad.wButtons += joystick.rgbButtons[12] ? 0x0400 : 0;

		switch ((int)(inputReport.inputBuffer[8 + inputReport.bluetooth] & 0x0f)) {
		case 0: ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP; break;

		case 1: ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_RIGHT; break;

		case 2: ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_RIGHT; break;

		case 3: ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_RIGHT; break;

		case 4: ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN; break;

		case 5: ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_LEFT; break;

		case 6: ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_LEFT; break;

		case 7: ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_LEFT; break;
		}


		ControllerState.Gamepad.bLeftTrigger = (int)inputReport.inputBuffer[5 + inputReport.bluetooth];
		ControllerState.Gamepad.bRightTrigger = (int)inputReport.inputBuffer[6 + inputReport.bluetooth];
		ControllerState.Gamepad.sThumbLX = (int)((inputReport.inputBuffer[1 + inputReport.bluetooth] * 257) - 32768);
		ControllerState.Gamepad.sThumbLY = (int)(32767 - (inputReport.inputBuffer[2 + inputReport.bluetooth] * 257));
		ControllerState.Gamepad.sThumbRX = (int)((inputReport.inputBuffer[3 + inputReport.bluetooth] * 257) - 32768);
		ControllerState.Gamepad.sThumbRY = (int)(32767 - (inputReport.inputBuffer[4 + inputReport.bluetooth] * 257));

		vigem_target_x360_update(client, emulateX360, *reinterpret_cast<XUSB_REPORT*>(&ControllerState.Gamepad));

	}

	//Cleanup

	vigem_target_remove(client, emulateX360);
	vigem_target_free(emulateX360);
	vigem_disconnect(client);
	vigem_free(client);


	return 0;
}
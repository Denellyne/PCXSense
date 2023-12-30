#include "util.h"
#include <thread>

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
	controller x360Controller{};

	x360Controller.client = vigem_alloc();

	LPVOID ptrController = &x360Controller;

#if EXPERIMENTAL
	x360Controller.rainbow = true;
#endif

#ifdef _DEBUG
	std::thread(asyncDataReport, std::ref(x360Controller)).detach(); // Displays controller info
#endif // _DEBUG

	if (initializeFakeController(x360Controller.emulateX360, x360Controller.target, x360Controller.client) != 0) return -1;
	vigem_target_x360_register_notification(x360Controller.client, x360Controller.emulateX360, &getRumble, ptrController);
	std::thread(sendOutputReport, std::ref(x360Controller)).detach();


	while (true) {

		XInputGetState(0, &x360Controller.ControllerState);

		getInputReport(x360Controller);
		
		vigem_target_x360_update(x360Controller.client, x360Controller.emulateX360, *reinterpret_cast<XUSB_REPORT*>(&x360Controller.ControllerState.Gamepad));
	}

	//Cleanup

	vigem_target_remove(x360Controller.client, x360Controller.emulateX360);
	vigem_target_free(x360Controller.emulateX360);
	vigem_disconnect(x360Controller.client);
	vigem_free(x360Controller.client);


	return 0;
}
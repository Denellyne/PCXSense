#include "main.h"
extern std::string Version = "PCXSenseBeta0.6";

int main() {
#ifndef _DEBUG
	autoUpdater();
#endif

	SetProcessShutdownParameters(2, 0);
	SetConsoleCtrlHandler(exitFunction, TRUE);
	
	//Initialize Fake Controller
	controller x360Controller{};

	std::vector<Macros> Macro;
	std::vector<gameProfile> gameProfiles;

	loadMacros(Macro);
	loadProfiles(gameProfiles);
	
	x360Controller.client = vigem_alloc();

	if (x360Controller.client == NULL) {
		if (MessageBox(NULL, L"The app couldn't start, please install VigemBusDriver ,if this error persists please open an issue on github", L"Vigem bus", MB_YESNO | MB_TASKMODAL) == IDNO) return -1;
		ShellExecute(0, 0, L"https://github.com/nefarius/ViGEmBus/releases/tag/v1.22.0", 0, 0, SW_SHOW);
		return -1;
	}
	
	if (initializeFakeController(x360Controller.emulateX360, x360Controller.target, x360Controller.client) != 0) {
		if (MessageBox(NULL, L"The app couldn't start, please install VigemBusDriver ,if this error persists please open an issue on github", L"Vigem bus", MB_YESNO | MB_TASKMODAL) == IDNO) return -1;
		ShellExecute(0, 0, L"https://github.com/nefarius/ViGEmBus/releases/tag/v1.22.0", 0, 0, SW_SHOW);
		return -1;
	}

	//Start async threads
	std::thread asyncOutputReport(sendOutputReport, std::ref(x360Controller));
	asyncOutputReport.detach();
	std::thread(GUI, std::ref(x360Controller),std::ref(Macro),std::ref(gameProfiles)).detach();
	std::thread(asyncMacro, std::ref(x360Controller),std::ref(Macro)).detach();
	std::thread(asyncGameProfile,std::ref(gameProfiles),std::ref(x360Controller)).detach();

#if _DEBUG
	//std::thread(debugData, std::ref(x360Controller)).detach(); // Displays controller info
#endif

	ptrController = &x360Controller;
	asyncThreadPointer = &asyncOutputReport;
	ptrMacros = &Macro;
	ptrProfiles = &gameProfiles;

	vigem_target_x360_register_notification(x360Controller.client, x360Controller.emulateX360, &getRumble, ptrController);

	while (true) {

		XInputGetState(0, &x360Controller.ControllerState);

		getInputReport(x360Controller);

		vigem_target_x360_update(x360Controller.client, x360Controller.emulateX360, *reinterpret_cast<XUSB_REPORT*>(&x360Controller.ControllerState.Gamepad));

	}

	return 0;
}


//Should fix performance

#pragma warning(disable:4073) // initializers put in library initialization area
#pragma init_seg(lib)

#if _MSC_VER < 2200
struct VS2013_threading_fix
{
	VS2013_threading_fix()
	{
		_Cnd_do_broadcast_at_thread_exit();
	}
} threading_fix;
#endif
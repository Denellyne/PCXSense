#include "main.h"
extern std::string Version = "PCXSenseBeta0.5";

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
	
	ptrController = &x360Controller;
	x360Controller.client = vigem_alloc();

	if (initializeFakeController(x360Controller.emulateX360, x360Controller.target, x360Controller.client) != 0) return -1;

	//Start async threads
	std::thread asyncOutputReport(sendOutputReport, std::ref(x360Controller));
	asyncOutputReport.detach();
	std::thread(GUI, std::ref(x360Controller),std::ref(Macro),std::ref(gameProfiles)).detach();
	std::thread(asyncMacro, std::ref(x360Controller),std::ref(Macro)).detach();
	std::thread(asyncGameProfile,std::ref(gameProfiles) , std::ref(x360Controller)).detach();

#if _DEBUG
	//std::thread(asyncDataReport, std::ref(x360Controller)).detach(); // Displays controller info
#endif

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


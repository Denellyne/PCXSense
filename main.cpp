#include "main.h"
extern std::string Version = "PCXSenseBeta0.4";

int main() {
#ifndef _DEBUG
	autoUpdater();
#endif

	SetProcessShutdownParameters(2, 0);
	SetConsoleCtrlHandler(exitFunction, TRUE);

	//Initialize Fake Controller
	controller x360Controller{};

	std::vector<Macros> Macro;
	std::vector<triggerProfile> triggers(2);
	triggers[0].name = "No Profile";
	memcpy(&ptrCurrentTriggerProfile, &triggers[0].trigger, 8);

	loadTrigger(triggers);
	loadMacros(Macro);
	

	ptrController = &x360Controller;
	x360Controller.client = vigem_alloc();

	if (initializeFakeController(x360Controller.emulateX360, x360Controller.target, x360Controller.client) != 0) return -1;

	//Start async threads
	std::thread asyncOutputReport(sendOutputReport, std::ref(x360Controller));
	asyncOutputReport.detach();
	std::thread(GUI, std::ref(x360Controller),std::ref(Macro),std::ref(triggers)).detach();
	std::thread(asyncMacro, std::ref(x360Controller),std::ref(Macro)).detach();

#if _DEBUG
	std::thread(asyncDataReport, std::ref(x360Controller)).detach(); // Displays controller info
#endif

	asyncThreadPointer = &asyncOutputReport;
	ptrTriggers = &triggers;
	ptrMacros = &Macro;

	vigem_target_x360_register_notification(x360Controller.client, x360Controller.emulateX360, &getRumble, ptrController);

	while (true) {

		XInputGetState(0, &x360Controller.ControllerState);

		getInputReport(x360Controller);
		
		vigem_target_x360_update(x360Controller.client, x360Controller.emulateX360, *reinterpret_cast<XUSB_REPORT*>(&x360Controller.ControllerState.Gamepad));
	}

	return 0;
}


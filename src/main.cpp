#include "main.h"
extern std::string Version = "PCXSenseBeta0.8";

extern void (*getInputs)(controller& x360Controller) = &getDualsenseInput;
extern std::string currentDirectory{};

//void inline secondcontroller(controller& secondcontroller) {
//	secondcontroller.client = vigem_alloc();
//	if (secondcontroller.client == null) {
//		if (messagebox(null, l"the app couldn't start, please install vigembusdriver ,if this error persists please open an issue on github", l"vigem bus", mb_yesno | mb_taskmodal) == idno) return;
//		shellexecute(0, 0, l"https://github.com/nefarius/vigembus/releases/tag/v1.22.0", 0, 0, sw_show);
//		return;
//	}
//
//	if (initializefakecontroller(secondcontroller.emulatex360, secondcontroller.target, secondcontroller.client) != 0) {
//		if (messagebox(null, l"the app couldn't start, please install vigembusdriver ,if this error persists please open an issue on github", l"vigem bus", mb_yesno | mb_taskmodal) == idno) return;
//		shellexecute(0, 0, l"https://github.com/nefarius/vigembus/releases/tag/v1.22.0", 0, 0, sw_show);
//		return;
//	}
//
//	if (!isdualshoch4connected(secondcontroller))
//		return;
//
//	getinputs = &getdualshock4input;
//	std::thread* asyncthreadpointer = new std::thread(senddualshock4outputreport, std::ref(secondcontroller));
//	asyncthreadpointer->detach();
//	
//	while(true){
//
//		xinputgetstate(1, &secondcontroller.controllerstate);
//
//		getinputs(secondcontroller);
//
//		vigem_target_x360_update(secondcontroller.client, secondcontroller.emulatex360, *reinterpret_cast<xusb_report*>(&secondcontroller.controllerstate.gamepad));
//	}
//
//}

int main(int argc,char* argv[]) {
	currentDirectory = std::filesystem::path(argv[0]).parent_path().string();
#ifdef NDEBUG
	autoUpdater();
#endif
	SetProcessShutdownParameters(2, 0);
	SetConsoleCtrlHandler(exitFunction, TRUE);
	
	//Initialize Fake Controller
	controller x360Controller{};
	//controller x360Controller2{};

	std::vector<Macros> Macro;
	std::vector<gameProfile> gameProfiles;

	loadMacros(Macro);
	loadProfiles(gameProfiles);
	triggerToProfile(gameProfiles);
	loadLightSettings(x360Controller);
	
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
	std::thread(GUI, std::ref(x360Controller),std::ref(Macro),std::ref(gameProfiles), startMinimized(argv[1])).detach();
	std::thread(asyncMacro, std::ref(x360Controller),std::ref(Macro)).detach();
	std::thread(asyncGameProfile,std::ref(gameProfiles),std::ref(x360Controller)).detach();
	//std::thread(secondController,std::ref(x360Controller2)).detach();

#if _DEBUG
	//std::thread(debugData, std::ref(x360Controller)).detach(); // Displays controller info
#endif

	ptrController = &x360Controller;
//	ptrController2 = &x360Controller2;
	ptrMacros = &Macro;
	ptrProfiles = &gameProfiles;

	vigem_target_x360_register_notification(x360Controller.client, x360Controller.emulateX360, &getRumble, ptrController);
	//vigem_target_x360_register_notification(x360Controller2.client, x360Controller2.emulateX360, &getRumble, ptrController2);
	hideDevice();
	isControllerConnected(x360Controller);
	while (true) {
		XInputGetState(0, &x360Controller.ControllerState);

		getInputs(x360Controller);

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
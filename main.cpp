#ifndef _DEBUG
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#include "util.h"
#include "GUI.h"
#include <thread>
#include <format>

LPVOID ptrController;
LPVOID asyncThreadPointer;
extern UCHAR rumble[2]{};
LPSTR Version = (char*)(" \"PCXSenseBeta0.2\"");
void inline update() {
	STARTUPINFOA si;
    PROCESS_INFORMATION pi; // The function returns this
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

	//CONST std::string commandLine = Version;
	//std::cout << commandLine;
	//Sleep(1000000000000);
    // Start the child process.
    if (!CreateProcessA(
        "PCXSenseUpdater.exe",      // app path
        Version,     // Command line 
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        throw std::exception("Could not create child process");
    }
    else
    {
        std::cout << "[          ] Successfully launched child process" << std::endl;
		Sleep(1000);

    }
	CloseHandle(pi.hThread);
}

VOID CALLBACK getRumble(PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, UCHAR LedNumber, LPVOID UserData)
{
	rumble[0] = SmallMotor;
	rumble[1] = LargeMotor;
}

void zeroOutputReport() {
	unsigned char outputHID[547]{};
	if (reinterpret_cast<controller*>(ptrController)->bluetooth) {
		ZeroMemory(outputHID, 547);

		outputHID[0] = 0x31;
		outputHID[1] = 0x02;
		outputHID[2] = 0x03 | 0x04 | 0x08;
		outputHID[3] = 0x55;

		const UINT32 crc = computeCRC32(outputHID, 74);

		outputHID[74] = (crc & 0x000000FF);
		outputHID[75] = ((crc & 0x0000FF00) >> 8UL);
		outputHID[76] = ((crc & 0x00FF0000) >> 16UL);
		outputHID[77] = ((crc & 0xFF000000) >> 24UL);

		WriteFile(reinterpret_cast<controller*>(ptrController)->deviceHandle, outputHID, 547, NULL, NULL);
	}
	else {
		ZeroMemory(outputHID, 547);

		outputHID[0] = 0x02;
		outputHID[1] = 0x03 | 0x04 | 0x08;
		outputHID[2] = 0x55;

		WriteFile(reinterpret_cast<controller*>(ptrController)->deviceHandle, outputHID, 64, NULL, NULL);
	}
}

extern BOOL WINAPI exitFunction(_In_ DWORD dwCtrlType) {
	reinterpret_cast<std::thread*>(asyncThreadPointer)->~thread();
	zeroOutputReport();

	//Cleanup	
	vigem_target_remove(reinterpret_cast<controller*>(ptrController)->client, reinterpret_cast<controller*>(ptrController)->emulateX360);
	vigem_target_free(reinterpret_cast<controller*>(ptrController)->emulateX360);
	vigem_disconnect(reinterpret_cast<controller*>(ptrController)->client);
	vigem_free(reinterpret_cast<controller*>(ptrController)->client);
	_exit(NULL);
	return TRUE;
}


int main() {
	update();
	Sleep(400);
	//Initialize Fake Controller
	controller x360Controller{};

	ptrController = &x360Controller;

	x360Controller.client = vigem_alloc();

	SetProcessShutdownParameters(2, 0);

	SetConsoleCtrlHandler(exitFunction, TRUE);

	if (initializeFakeController(x360Controller.emulateX360, x360Controller.target, x360Controller.client) != 0) return -1;

	std::thread asyncOutputReport(sendOutputReport, std::ref(x360Controller));
	asyncOutputReport.detach();

	asyncThreadPointer = &asyncOutputReport;
#if EXPERIMENTAL
	std::thread(GUI,std::ref(x360Controller)).detach();
#endif
	vigem_target_x360_register_notification(x360Controller.client, x360Controller.emulateX360, &getRumble, ptrController);

#ifndef _DEBUG
	std::thread(asyncDataReport, std::ref(x360Controller)).detach(); // Displays controller info
#endif // _DEBUG

	while (true) {

		XInputGetState(0, &x360Controller.ControllerState);

		getInputReport(x360Controller);
		
		vigem_target_x360_update(x360Controller.client, x360Controller.emulateX360, *reinterpret_cast<XUSB_REPORT*>(&x360Controller.ControllerState.Gamepad));
	}
	return 0;
}


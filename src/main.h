#pragma once
#ifdef NDEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#else
#include "misc\util.h"
#endif
#include "GUI\GUI.h"
#include "Updater\update.h"
#include "Startup/startup.h"
#include "User Settings/Macros/macro.h"
#include "User Settings/Adaptive Triggers/Adaptive Triggers.h"
#include "User Settings/Game Profiles/gameProfile.h"
#include "User Settings/Game Profiles/saveLoad.h"
#include "User Settings/Lightbar/Lightbar.h"
#include "ControllerIO/Device Hiding/deviceHiding.h"
#include <thread>
#include <format>

LPVOID ptrController;
LPVOID ptrController2;
extern LPVOID asyncThreadPointer = nullptr;
LPVOID ptrMacros;
LPVOID ptrProfiles;
extern UCHAR rumble[2]{};

VOID CALLBACK getRumble(PVIGEM_CLIENT Client, PVIGEM_TARGET Target, UCHAR LargeMotor, UCHAR SmallMotor, UCHAR LedNumber, LPVOID UserData){
	rumble[0] = SmallMotor;
	rumble[1] = LargeMotor;
}

void zeroOutputReport() {
	unsigned char outputHID[547]{};
	if (reinterpret_cast<controller*>(ptrController)->hidOffset) {
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
#ifndef NDEBUG
	extern void saveOnExit();
	saveOnExit();
#endif // !NDEBUG

	if (reinterpret_cast<std::thread*>(asyncThreadPointer) != nullptr) {
		reinterpret_cast<controller*>(ptrController)->threadStop = true;
		delete asyncThreadPointer;
		asyncThreadPointer = nullptr;
	}
	zeroOutputReport();
	saveMacros(*reinterpret_cast<std::vector<Macros>*>(ptrMacros));
	saveLightSettings(*reinterpret_cast<controller*>(ptrController));
	saveProfiles(*reinterpret_cast<std::vector<gameProfile>*>(ptrProfiles));

	//Cleanup	
	vigem_target_remove(reinterpret_cast<controller*>(ptrController)->client, reinterpret_cast<controller*>(ptrController)->emulateX360);
	vigem_target_free(reinterpret_cast<controller*>(ptrController)->emulateX360);
	vigem_disconnect(reinterpret_cast<controller*>(ptrController)->client);
	vigem_free(reinterpret_cast<controller*>(ptrController)->client);
	_exit(NULL);
	return TRUE;
}

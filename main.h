#pragma once
#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include "misc\util.h"
#include "GUI\GUI.h"
#include "Updater\update.h"
#include "User Settings/Macros/macro.h"
#include "User Settings/Adaptive Triggers/Adaptive Triggers.h"
#include "User Settings/Game Profiles/gameProfile.h"
#include <thread>
#include <format>

LPVOID ptrController;
LPVOID asyncThreadPointer;
LPVOID ptrMacros;
LPVOID ptrProfiles;
extern UCHAR rumble[2]{};


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
	saveProfiles(*reinterpret_cast<std::vector<gameProfile>*>(ptrProfiles));
	saveMacros(*reinterpret_cast<std::vector<Macros>*>(ptrMacros));

	//Cleanup	
	vigem_target_remove(reinterpret_cast<controller*>(ptrController)->client, reinterpret_cast<controller*>(ptrController)->emulateX360);
	vigem_target_free(reinterpret_cast<controller*>(ptrController)->emulateX360);
	vigem_disconnect(reinterpret_cast<controller*>(ptrController)->client);
	vigem_free(reinterpret_cast<controller*>(ptrController)->client);
	_exit(NULL);
	return TRUE;
}


/*
	To add:
	Make so macros can Enable AT
	//Create the macro inside of the AT profile

	Turn off bluetooth on USB

	Enable custom lightbar

	Per Game Profile

	Make so Rumble Motor Speed can affect the triggers



*/
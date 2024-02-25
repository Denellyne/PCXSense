#include "controllerIO.h"
#include <tlhelp32.h>
#include "User Settings/Adaptive Triggers/Adaptive Triggers.h"
#include "User Settings/Game Profiles/gameProfile.h"
#include "Controller Connections/controllerConnections.h"
#include "Dualsense/dualsense.h"
#include "Dualshock4/dualshock4.h"
#include "thread"
#if (defined _DEBUG || defined _PROFILE)
#include "Misc/benchmark.h"
#else
#define DEBUG(x) do ; while(0)
#endif // _DEBUG


//#define boolSetter(x,y,operatorA,operatorB) x * (x operatorA y) + y * (y operatorB x);

extern "C" int returnSmaller(int x); //Assembly Function in src/Assembly Functions/assemblyFunctions.s

bool inline isControllerConnected(controller& x360Controller) {
	x360Controller.isConnected = false;

	//Stop output thread
	extern LPVOID asyncThreadPointer;
	if (reinterpret_cast<std::thread*>(asyncThreadPointer) != nullptr) {
		delete asyncThreadPointer;
		asyncThreadPointer = nullptr;
	}

	while (true) {
		Sleep(50); //Sleeps for 50ms so it doesnt spam the cpu

		extern void (*getInputs)(controller & x360Controller);

		if (isDualsenseConnected(x360Controller)) {
			x360Controller.threadStop = false;
			getInputs = &getDualsenseInput;
			asyncThreadPointer = new std::thread(sendDualsenseOutputReport, std::ref(x360Controller));
			reinterpret_cast<std::thread*>(asyncThreadPointer)->detach();
			return true;
		}
		if (isDualsenseEdgeConnected(x360Controller)) {
			x360Controller.threadStop = false;
			getInputs = &getDualsenseInput;
			asyncThreadPointer = new std::thread(sendDualsenseOutputReport, std::ref(x360Controller));
			reinterpret_cast<std::thread*>(asyncThreadPointer)->detach();
			return true;
		}
		if (isDualShock4Connected(x360Controller)) {
			x360Controller.threadStop = false;
			getInputs = &getDualShock4Input;
			asyncThreadPointer = new std::thread(sendDualShock4OutputReport, std::ref(x360Controller));
			reinterpret_cast<std::thread*>(asyncThreadPointer)->detach();
			return true;
		}
	}

}

uint32_t computeCRC32(unsigned char* buffer, const size_t& len)
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

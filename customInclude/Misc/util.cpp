#pragma once
#include "Misc\util.h"

#ifdef _DEBUG
void debugData(controller &x360Controller) {

	while (true) {

		Sleep(20);
		system("cls"); //Clear console

		std::cout << "\nLeftJoystick Horizontal Value: " << (int)((x360Controller.inputBuffer[1 + x360Controller.bluetooth] * 257) - 32768) << '\n';
		std::cout << "LeftJoystick Vertical Value: " << (int)(32767 - (x360Controller.inputBuffer[2 + x360Controller.bluetooth] * 257)) << '\n';
		std::cout << "RightJoystick Horizontal Value: " << (int)((x360Controller.inputBuffer[3 + x360Controller.bluetooth] * 257) - 32768) << '\n';
		std::cout << "RightJoystick Vertical Value: " << (int)(32767 - (x360Controller.inputBuffer[4 + x360Controller.bluetooth] * 257)) << '\n';
		std::cout << "Left Trigger Value: " << (int)x360Controller.ControllerState.Gamepad.bLeftTrigger << '\n';
		std::cout << "Right Trigger Value: " << (int)x360Controller.ControllerState.Gamepad.bRightTrigger << '\n';
		std::cout << "Battery Level: " << x360Controller.batteryLevel << "%\n";
		std::cout << "Buttons Reading: " << x360Controller.ControllerState.Gamepad.wButtons << "\n";

		switch ((int)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & 0x0f)) {

		case 0: DEBUG("Dpad Up"); break;

		case 1: DEBUG("Dpad Up and Dpad Right"); break;

		case 2: DEBUG("Dpad Right"); break;

		case 3: DEBUG("Dpad Down and Dpad Right"); break;

		case 4: DEBUG("Dpad Down"); break;

		case 5: DEBUG("Dpad Down and Dpad Left"); break;

		case 6: DEBUG("Dpad Left"); break;

		case 7: DEBUG("Dpad Up and Dpad Left"); break;
		}

		if ((bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 4))) DEBUG("Square Button\n");

		if ((bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 5))) DEBUG("Cross Button\n");

		if ((bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 6))) DEBUG("Circle Button\n");

		if ((bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 7))) DEBUG("Triangle Button\n");

		if ((bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 0))) DEBUG("L1 Button\n");

		if ((bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 1))) DEBUG("R1 Button\n");

		if ((bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 4))) DEBUG("Select Button\n");

		if ((bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 5))) DEBUG("Start Button\n");

		if ((bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 6))) DEBUG("L3 Button\n");

		if ((bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 7))) DEBUG("R3 Button\n");

		if ((bool)(x360Controller.inputBuffer[10 + x360Controller.bluetooth] & (1 << 0))) DEBUG("Sony/Home Button\n");

		if ((bool)(x360Controller.inputBuffer[10 + x360Controller.bluetooth] & 0x02)) DEBUG("Toutchpad Click\n");

		if (!x360Controller.isConnected)
			DEBUG("Failed to get device state");
			
		
	}
}
#endif





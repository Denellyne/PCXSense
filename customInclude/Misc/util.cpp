#pragma once
#include "Misc\util.h"
#include <tlhelp32.h>

bool isControllerConnected(controller& inputReport) {
	Sleep(50);
	hid_device_info* deviceInfo = hid_enumerate(DS_VENDOR_ID, DS_PRODUCT_ID);
	if (deviceInfo == nullptr) {
		inputReport.isConnected = false;
		return false;
	}
	inputReport.deviceHandle = CreateFileA(deviceInfo->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	inputReport.bluetooth = deviceInfo->interface_number == -1;

	if (inputReport.bluetooth) {
		inputReport.bufferSize = 78;
		inputReport.inputBuffer[0] = 0x31;
	}
	else {
		inputReport.bufferSize = 64;
		inputReport.inputBuffer[0] = 0x01;
	}
	hid_free_enumeration(deviceInfo);

	if ((bool)inputReport.deviceHandle) {
		inputReport.isConnected = true;
		return true;
	}
	else inputReport.isConnected = false;

	return false;
}

void asyncDataReport(controller &x360Controller) {

	while (true) {

		Sleep(1);
		system("cls"); //Clear console

		std::cout << "\nLeftJoystick Horizontal Value: " << (int)((x360Controller.inputBuffer[1 + x360Controller.bluetooth] * 257) - 32768) << '\n';
		std::cout << "LeftJoystick Vertical Value: " << (int)(32767 - (x360Controller.inputBuffer[2 + x360Controller.bluetooth] * 257)) << '\n';
		std::cout << "RightJoystick Horizontal Value: " << (int)((x360Controller.inputBuffer[3 + x360Controller.bluetooth] * 257) - 32768) << '\n';
		std::cout << "RightJoystick Vertical Value: " << (int)(32767 - (x360Controller.inputBuffer[4 + x360Controller.bluetooth] * 257)) << '\n';
		std::cout << "Left Trigger Value: " << (int)x360Controller.ControllerState.Gamepad.bLeftTrigger << '\n';
		std::cout << "Right Trigger Value: " << (int)x360Controller.ControllerState.Gamepad.bRightTrigger << '\n';
		std::cout << "Battery Level: " << x360Controller.batteryLevel << "%\n";

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

		
		while (!x360Controller.isConnected) {
			std::cout << "Failed to get Device State\n";
			std::cout << "Reconnecting";
			Sleep(500);
			std::cout << '.';
			Sleep(500);
			std::cout << '.';
			Sleep(500);
			std::cout << '.';
			Sleep(500);
			system("cls");
		}	
		
	}
}

int initializeFakeController(PVIGEM_TARGET& emulateX360, VIGEM_ERROR& target, PVIGEM_CLIENT& client) {

	if (client == nullptr)
	{
		std::cerr << "Uh, not enough memory to do that?!" << std::endl;
		return -1;
	}

	const auto retval = vigem_connect(client);

	if (!VIGEM_SUCCESS(retval))
	{
		std::cerr << "ViGEm Bus connection failed with error code: 0x" << std::hex << retval << std::endl;
		return -1;
	}

	emulateX360 = vigem_target_x360_alloc();

	target = vigem_target_add(client, emulateX360);

	return 0;
}

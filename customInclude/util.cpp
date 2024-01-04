#pragma once
#include "util.h"
#include <tlhelp32.h>


bool isControllerConnected(controller& inputReport) {
	//Sleep(500);
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

void inline isEmulatorRunning(unsigned char* outputHID,int bluetooth,int& shortTriggers){
	PROCESSENTRY32 entry{};
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry)) {
			if (!wcsicmp(entry.szExeFile, L"Dolphin.exe")) {
				outputHID[11 + bluetooth] = 0x2; //Mode Motor Right
				outputHID[12 + bluetooth] = 0x90; //right trigger start of resistance section
				outputHID[13 + bluetooth] = 0xA0; //right trigger (mode1) amount of force exerted (mode2) end of resistance section supplemental mode 4+20) flag(s?) 0x02 = do not pause effect when fully presse
				outputHID[14 + bluetooth] = 0xFF; //right trigger force exerted in range (mode2)
				//	outputHID[15 + bluetooth] = 0x0; // strength of effect near release state (requires supplement modes 4 and 20)
					//outputHID[16 + bluetooth] = 0x0; // strength of effect near middle (requires supplement modes 4 and 20)
				//	outputHID[17 + bluetooth] = 0x0; // strength of effect at pressed state (requires supplement modes 4 and 20)
			//		outputHID[20 + bluetooth] = 0x0; // effect actuation frequency in Hz (requires supplement modes 4 and 20)

				outputHID[22 + bluetooth] = 0x2; //Mode Motor Left
				outputHID[23 + bluetooth] = 0x90; //Left trigger start of resistance section
				outputHID[24 + bluetooth] = 0xA0; //Left trigger (mode1) amount of force exerted (mode2) end of resistance section supplemental mode 4+20) flag(s?) 0x02 = do not pause effect when fully presse
				outputHID[25 + bluetooth] = 0xFF; //Left trigger force exerted in range (mode2)
				//	outputHID[26 + bluetooth] = 0x0; // strength of effect near release state (requires supplement modes 4 and 20)
				//	outputHID[27 + bluetooth] = 0x0; // strength of effect near middle (requires supplement modes 4 and 20)
				//	outputHID[28 + bluetooth] = 0x0; // strength of effect at pressed state (requires supplement modes 4 and 20)
				//	outputHID[31 + bluetooth] = 0x0; // effect actuation frequency in Hz (requires supplement modes 4 and 20)

			}
			if (!wcsicmp(entry.szExeFile, L"Yuzu.exe")) {
				shortTriggers = 190;
				outputHID[11 + bluetooth] = 0x2 ;
				outputHID[12 + bluetooth] = 40;
				outputHID[13 + bluetooth] = 100;
				outputHID[14 + bluetooth] = 35;

				outputHID[22 + bluetooth] = 0x2;
				outputHID[23 + bluetooth] = 20;
				outputHID[24 + bluetooth] = 180;
				outputHID[25 + bluetooth] = 50;
				CloseHandle(snapshot);
				return;
			}
			
		}
	shortTriggers = 0;
				
	CloseHandle(snapshot);
}

uint32_t computeCRC32(unsigned char* buffer,const size_t& len)
{
	UINT32 result = crcSeed;
	for (size_t i = 0; i < len; i++)
		// Compute crc
		result = hashTable[((unsigned char)result) ^ ((unsigned char)buffer[i])] ^ (result >> 8);

	// Return result
	return result;
}

void extern inline sendOutputReport(controller& x360Controller) {
	float Red{ 210 }, Green{}, Blue{ 90 };
	bool AddRed{ true }, AddGreen{ true }, AddBlue{ true };
	unsigned char outputHID[547]{};

	while (true) {
		Sleep(4);
		if (x360Controller.bluetooth) {
			ZeroMemory(outputHID, 547);


			outputHID[0] = 0x31;
			outputHID[1] = 0x02;
			outputHID[2] = 0x03 | 0x04 | 0x08;
			outputHID[3] = 0x55;

			outputHID[4] = rumble[0]; //Low Rumble
			outputHID[5] = rumble[1]; //High Rumble

			outputHID[10] = 0x00;
			outputHID[40] = 0x02;
			outputHID[43] = 0x02;
			outputHID[44] = 0x02;

			switch (x360Controller.batteryLevel) {
			case 0:
				outputHID[10] = 0x02;
				outputHID[46] = 255; //Red
				break;
			case 12:
				outputHID[10] = 0x01;
				outputHID[46] = 200; //Red
				outputHID[47] = 0; //Green
				outputHID[48] = 55; //Blue
				break;
			case 27:
				outputHID[46] = 230; //Red
				outputHID[47] = 90; //Green
				outputHID[48] = 0; //Blue
				break;
			case 37:
				outputHID[46] = 230; //Red
				outputHID[47] = 130; //Green
				outputHID[48] = 0; //Blue
				break;
			case 50:
				outputHID[46] = 140; //Red
				outputHID[47] = 30; //Green
				outputHID[48] = 90; //Blue
				break;
			case 67:
				outputHID[46] = 90; //Red
				outputHID[47] = 0; //Green
				outputHID[48] = 140; //Blue
				break;
			case 75:
				outputHID[46] = 90; //Red
				outputHID[47] = 0; //Green
				outputHID[48] = 80; //Blue
				break;
			case 100:
				outputHID[46] = 200; //Red
				outputHID[47] = 0; //Green
				outputHID[48] = 229; //Blue
				break;
			default:
				outputHID[46] = 255; //Red
				outputHID[47] = 140; //Green
				outputHID[48] = 0; //Blue
				break;
			}
			
#if EXPERIMENTAL
			isEmulatorRunning(outputHID,x360Controller.bluetooth,x360Controller.shortTriggers);

			if (x360Controller.rainbow) {
				if (Red == 255) AddRed = false;
				if (Red == 0) AddRed = true;
				if (Green == 255) AddGreen = false;
				if (Green == 0) AddGreen = true;
				if (Blue == 255) AddBlue = false;
				if (Blue == 0) AddBlue = true;

				if (AddRed) Red ++;
				else Red --;
				if (AddGreen) Green ++;
				else Green --;
				if (AddBlue) Blue ++;
				else Blue --;

				outputHID[46] = Red; //Red
				outputHID[47] = Green; //Green
				outputHID[48] = Blue; //Blue
			}
#endif // EXPERIMENTAL

			const UINT32 crc = computeCRC32(outputHID, 74);

			outputHID[74] = (crc & 0x000000FF);
			outputHID[75] = ((crc & 0x0000FF00) >> 8UL);
			outputHID[76] = ((crc & 0x00FF0000) >> 16UL);
			outputHID[77] = ((crc & 0xFF000000) >> 24UL);

			x360Controller.RGB.red = outputHID[46];
			x360Controller.RGB.green = outputHID[47];
			x360Controller.RGB.blue = outputHID[48];


			WriteFile(x360Controller.deviceHandle, outputHID, 547, NULL, NULL);
		}
		else {
			ZeroMemory(outputHID, 547);

			outputHID[0] = 0x02;
			outputHID[1] = 0x03 | 0x04 | 0x08;
			outputHID[2] = 0x55;

			outputHID[3] = rumble[0]; //Low Rumble
			outputHID[4] = rumble[1]; //High Rumble

			outputHID[9] = 0x00; //LED Controls
			outputHID[39] = 0x02;
			outputHID[42] = 0x02;
			outputHID[43] = 0x02;

			switch (x360Controller.batteryLevel) {
			case 70:
				outputHID[45] = 20; //Red
				outputHID[46] = 170; //Green
				outputHID[47] = 150; //Blue
				break;
			case 87:
				outputHID[45] = 20; //Red
				outputHID[46] = 170; //Green
				outputHID[47] = 150; //Blue
				break;
			case 100:
				outputHID[45] = 130; //Red
				outputHID[46] = 30; //Green
				outputHID[47] = 255; //Blue
				break;
			default:
				outputHID[45] = 255; //Red
				outputHID[46] = 140; //Green
				outputHID[47] = 0; //Blue
				break;
			}

#if EXPERIMENTAL
			isEmulatorRunning(outputHID,x360Controller.bluetooth,x360Controller.shortTriggers);

			if (x360Controller.rainbow) {
				if (Red == 255) AddRed = false;
				if (Red == 0) AddRed = true;
				if (Green == 255) AddGreen = false;
				if (Green == 0) AddGreen = true;
				if (Blue == 255) AddBlue = false;
				if (Blue == 0) AddBlue = true;

				if (AddRed) Red++;
				else Red--;
				if (AddGreen) Green++;
				else Green--;
				if (AddBlue) Blue++;
				else Blue--;

				outputHID[45] = Red; //Red
				outputHID[46] = Green; //Green
				outputHID[47] = Blue; //Blue
			}
#endif
			x360Controller.RGB.red = outputHID[45];
			x360Controller.RGB.green = outputHID[46];
			x360Controller.RGB.blue = outputHID[47];

			WriteFile(x360Controller.deviceHandle, outputHID, 64, NULL, NULL);
		}
	}
	
}

void inline getInputReport(controller& x360Controller){


		bool readSuccess = ReadFile(x360Controller.deviceHandle, x360Controller.inputBuffer, x360Controller.bufferSize, NULL, NULL);

		x360Controller.batteryLevel = (x360Controller.inputBuffer[53 + x360Controller.bluetooth] & 15) * 12.5; /* Hex 0x35 (USB) to get Battery / Hex 0x36 (Bluetooth) to get Battery
																									     because if bluetooth == true then bluetooth == 1 so we can just add bluetooth
																										 to the hex value of USB to get the battery reading
																										 */
		
		if (bool(x360Controller.shortTriggers)) {
			x360Controller.ControllerState.Gamepad.bLeftTrigger = (x360Controller.inputBuffer[5 + x360Controller.bluetooth])*0.25 + 190;
			x360Controller.ControllerState.Gamepad.bRightTrigger = (x360Controller.inputBuffer[6 + x360Controller.bluetooth]) * 0.25 + 190;
		}
		else {
			x360Controller.ControllerState.Gamepad.bLeftTrigger = x360Controller.inputBuffer[5 + x360Controller.bluetooth];
			x360Controller.ControllerState.Gamepad.bRightTrigger = x360Controller.inputBuffer[6 + x360Controller.bluetooth];
		}
		

		x360Controller.ControllerState.Gamepad.sThumbLX = ((x360Controller.inputBuffer[1 + x360Controller.bluetooth] * 257) - 32768);
		x360Controller.ControllerState.Gamepad.sThumbLY = (32767 - (x360Controller.inputBuffer[2 + x360Controller.bluetooth] * 257));
		x360Controller.ControllerState.Gamepad.sThumbRX = ((x360Controller.inputBuffer[3 + x360Controller.bluetooth] * 257) - 32768);
		x360Controller.ControllerState.Gamepad.sThumbRY = (32767 - (x360Controller.inputBuffer[4 + x360Controller.bluetooth] * 257));

		x360Controller.ControllerState.Gamepad.wButtons = (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 4)) ? XINPUT_GAMEPAD_X : 0;

		x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 5)) ? XINPUT_GAMEPAD_A : 0;

		x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 6)) ? XINPUT_GAMEPAD_B : 0;

		x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & (1 << 7)) ? XINPUT_GAMEPAD_Y : 0;

		x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 0)) ? XINPUT_GAMEPAD_LEFT_SHOULDER : 0;

		x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 1)) ? XINPUT_GAMEPAD_RIGHT_SHOULDER : 0;

		x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 4)) ? XINPUT_GAMEPAD_BACK : 0;

		x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 5)) ? XINPUT_GAMEPAD_START : 0;

		x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 6)) ? XINPUT_GAMEPAD_LEFT_THUMB : 0;

		x360Controller.ControllerState.Gamepad.wButtons += (bool)(x360Controller.inputBuffer[9 + x360Controller.bluetooth] & (1 << 7)) ? XINPUT_GAMEPAD_RIGHT_THUMB : 0;
		//ControllerState.Gamepad.wButtons += joystick.rgbButtons[12] ? 0x0400 : 0;

		switch ((int)(x360Controller.inputBuffer[8 + x360Controller.bluetooth] & 0x0f)) {
		case 0: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP; break;

		case 1: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_RIGHT; break;

		case 2: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_RIGHT; break;

		case 3: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_RIGHT; break;

		case 4: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN; break;

		case 5: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_DOWN + XINPUT_GAMEPAD_DPAD_LEFT; break;

		case 6: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_LEFT; break;

		case 7: x360Controller.ControllerState.Gamepad.wButtons += XINPUT_GAMEPAD_DPAD_UP + XINPUT_GAMEPAD_DPAD_LEFT; break;
		}

		if (!readSuccess) {
			CloseHandle(x360Controller.deviceHandle);
			while (!isControllerConnected(x360Controller)) {}
		}
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
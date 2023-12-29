#pragma once
#include "util.h"

#define DS_STATUS_BATTERY_CAPACITY 0xF
#define DS_STATUS_CHARGING 0xF0
#define DS_STATUS_CHARGING_SHIFT 4
#define DS_VENDOR_ID 0x054c
#define DS_PRODUCT_ID 0x0ce6

uint32_t computeCRC32(unsigned char* buffer, size_t len)
{
	UINT32 result = crcSeed;
	for (size_t i = 0; i < len; i++)
		// Compute crc
		result = hashTable[((unsigned char)result) ^ ((unsigned char)buffer[i])] ^ (result >> 8);

	// Return result
	return result;
}

void asyncSendOutputReport(inputReport& inputReport) {
	hid_device_info* deviceInfo = hid_enumerate(DS_VENDOR_ID, DS_PRODUCT_ID);

	HANDLE dualsense = CreateFileA(deviceInfo->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

	hid_free_enumeration(deviceInfo);

	unsigned char outputHID[547]{};
	while(true){
		if (inputReport.bluetooth) {
			ZeroMemory(outputHID, 547);

			outputHID[0] = 0x31;
			outputHID[1] = 0x02;
			outputHID[2] = 0x03;
			outputHID[3] = 0x55;

			outputHID[4] = rumble[0]; //Low Rumble
			outputHID[5] = rumble[1]; //High Rumble

			outputHID[10] = 0x00;
			outputHID[40] = 0x02;
			outputHID[43] = 0x02;
			outputHID[44] = 0x02;

			switch (inputReport.batteryLevel) {
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
			if (inputReport.gamecube) {
				outputHID[12] = 0x2; //Mode Motor Right
				outputHID[13] = 0x90; //right trigger start of resistance section
				outputHID[14] = 0xA0; //right trigger (mode1) amount of force exerted (mode2) end of resistance section supplemental mode 4+20) flag(s?) 0x02 = do not pause effect when fully presse
				outputHID[15] = 0xFF; //right trigger force exerted in range (mode2)
				outputHID[16] = 0x0; // strength of effect near release state (requires supplement modes 4 and 20)
				outputHID[17] = 0x0; // strength of effect near middle (requires supplement modes 4 and 20)
				outputHID[18] = 0x0; // strength of effect at pressed state (requires supplement modes 4 and 20)
				outputHID[21] = 0x0; // effect actuation frequency in Hz (requires supplement modes 4 and 20)


				outputHID[23] = 0x2; //Mode Motor Right
				outputHID[24] = 0x90; //right trigger start of resistance section
				outputHID[25] = 0xA0; //right trigger (mode1) amount of force exerted (mode2) end of resistance section supplemental mode 4+20) flag(s?) 0x02 = do not pause effect when fully presse
				outputHID[26] = 0xFF; //right trigger force exerted in range (mode2)
				outputHID[27] = 0x0; // strength of effect near release state (requires supplement modes 4 and 20)
				outputHID[28] = 0x0; // strength of effect near middle (requires supplement modes 4 and 20)
				outputHID[28] = 0x0; // strength of effect at pressed state (requires supplement modes 4 and 20)
				outputHID[31] = 0x0; // effect actuation frequency in Hz (requires supplement modes 4 and 20)
			}

			const UINT32 crc = computeCRC32(outputHID, 74);

			outputHID[74] = (crc & 0x000000FF);
			outputHID[75] = ((crc & 0x0000FF00) >> 8UL);
			outputHID[76] = ((crc & 0x00FF0000) >> 16UL);
			outputHID[77] = ((crc & 0xFF000000) >> 24UL);


			if (!(WriteFile(dualsense, outputHID, 547, NULL, NULL))) std::cout << GetLastError() << '\n';
		}
		else {
			ZeroMemory(outputHID, 64);


			outputHID[0] = 0x02;
			outputHID[1] = 0x03 | 0x04 | 0x08;
			outputHID[2] = 0x55;

			outputHID[3] = rumble[0]; //Low Rumble
			outputHID[4] = rumble[1]; //High Rumble

			outputHID[9] = 0x00;
			outputHID[39] = 0x02;
			outputHID[42] = 0x02;
			outputHID[43] = 0x02;

			switch (inputReport.batteryLevel) {
				case 70:
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

			if (inputReport.gamecube) {
				outputHID[11] = 0x2; //Mode Motor Right
				outputHID[12] = 0x90; //right trigger start of resistance section
				outputHID[13] = 0xA0; //right trigger (mode1) amount of force exerted (mode2) end of resistance section supplemental mode 4+20) flag(s?) 0x02 = do not pause effect when fully presse
				outputHID[14] = 0xFF; //right trigger force exerted in range (mode2)
				outputHID[15] = 0x0; // strength of effect near release state (requires supplement modes 4 and 20)
				outputHID[16] = 0x0; // strength of effect near middle (requires supplement modes 4 and 20)
				outputHID[17] = 0x0; // strength of effect at pressed state (requires supplement modes 4 and 20)
				outputHID[20] = 0x0; // effect actuation frequency in Hz (requires supplement modes 4 and 20)


				outputHID[22] = 0x2; //Mode Motor Right
				outputHID[23] = 0x90; //right trigger start of resistance section
				outputHID[24] = 0xA0; //right trigger (mode1) amount of force exerted (mode2) end of resistance section supplemental mode 4+20) flag(s?) 0x02 = do not pause effect when fully presse
				outputHID[25] = 0xFF; //right trigger force exerted in range (mode2)
				outputHID[26] = 0x0; // strength of effect near release state (requires supplement modes 4 and 20)
				outputHID[27] = 0x0; // strength of effect near middle (requires supplement modes 4 and 20)
				outputHID[28] = 0x0; // strength of effect at pressed state (requires supplement modes 4 and 20)
				outputHID[31] = 0x0; // effect actuation frequency in Hz (requires supplement modes 4 and 20)
			}
			if (!(WriteFile(dualsense, outputHID, 64, NULL, NULL))) std::cout << GetLastError() << '\n';
		}
	}
	CloseHandle(dualsense);
}
void inline asyncGetInputReport(inputReport& inputReport){

	hid_device_info* deviceInfo = hid_enumerate(DS_VENDOR_ID, DS_PRODUCT_ID);
	inputReport.bluetooth = deviceInfo->interface_number == -1;
	hid_device* dualsense = hid_open(DS_VENDOR_ID, DS_PRODUCT_ID, deviceInfo->serial_number);
	hid_free_enumeration(deviceInfo);

	if (inputReport.bluetooth) {
		inputReport.bufferSize = 78;
		inputReport.inputBuffer[0] = 0x31;
		hid_get_input_report(dualsense, inputReport.inputBuffer, inputReport.bufferSize);
	}
	else {
		inputReport.bufferSize = 64;
		inputReport.inputBuffer[0] = 0x01;
		hid_read(dualsense, inputReport.inputBuffer, inputReport.bufferSize);
	}
	inputReport.batteryLevel = (inputReport.inputBuffer[0x35 + inputReport.bluetooth] & 0x0F) * 12.5;

	while (true) {
		if (inputReport.bluetooth) hid_get_input_report(dualsense, inputReport.inputBuffer, inputReport.bufferSize);
		else hid_read(dualsense, inputReport.inputBuffer, inputReport.bufferSize);
	}
	hid_close(dualsense);
}

void asyncDataReport(inputReport &inputReport) {

	while (true) {
		
		Sleep(1);
		system("cls"); //Clear console

		switch ((int)(inputReport.inputBuffer[8 + inputReport.bluetooth] & 0x0f)) {
		case 0:
			DEBUG("Dpad Up");
			break;

		case 1:
			DEBUG("Dpad Up and Dpad Right");
			break;

		case 2:
			DEBUG("Dpad Right");
			break;

		case 3:
			DEBUG("Dpad Down and Dpad Right");
			break;

		case 4:
			DEBUG("Dpad Down");
			break;

		case 5:
			DEBUG("Dpad Down and Dpad Left");
			break;

		case 6:
			DEBUG("Dpad Left");
			break;

		case 7:
			DEBUG("Dpad Up and Dpad Left");
			break;
		}

		if ((bool)(inputReport.inputBuffer[8 + inputReport.bluetooth] & (1 << 4))) DEBUG("Square Button\n");

		if ((bool)(inputReport.inputBuffer[8 + inputReport.bluetooth] & (1 << 5))) DEBUG("X Button\n");

		if ((bool)(inputReport.inputBuffer[8 + inputReport.bluetooth] & (1 << 6))) DEBUG("Circle Button\n");

		if ((bool)(inputReport.inputBuffer[8 + inputReport.bluetooth] & (1 << 7))) DEBUG("Triangle Button\n");

		if ((bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 0))) DEBUG("L1 Button\n");

		if ((bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 1))) DEBUG("R1 Button\n");

		if ((bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 4))) DEBUG("Select Button\n");

		if ((bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 5))) DEBUG("Start Button\n");

		if ((bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 6))) DEBUG("L3 Button\n");

		if ((bool)(inputReport.inputBuffer[9 + inputReport.bluetooth] & (1 << 7))) DEBUG("R3 Button\n");

		if ((bool)(inputReport.inputBuffer[10 + inputReport.bluetooth] & (1 << 0))) DEBUG("Sony/Home Button\n");

		if ((bool)(inputReport.inputBuffer[10 + inputReport.bluetooth] & 0x02)) DEBUG("Toutchpad Click\n");

		if (false) {
			while (true) { //Controller is being acquired in the main function so we only need to check GetDeviceState is DI_OK
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
		std::cout << "\nLeftJoystick Horizontal Value: " << (int)((inputReport.inputBuffer[1 + inputReport.bluetooth] * 257) - 32768) << '\n';
		std::cout << "LeftJoystick Vertical Value: " << (int)(32767 - (inputReport.inputBuffer[2 + inputReport.bluetooth] * 257)) << '\n';
		std::cout << "RightJoystick Horizontal Value: " << (int)((inputReport.inputBuffer[3 + inputReport.bluetooth] * 257) - 32768) << '\n';
		std::cout << "RightJoystick Vertical Value: " << (int)(32767 - (inputReport.inputBuffer[4 + inputReport.bluetooth] * 257)) << '\n';
		std::cout << "Left Trigger Value: " << (int)inputReport.inputBuffer[5 + inputReport.bluetooth] << '\n';
		std::cout << "Right Trigger Value: " << (int)inputReport.inputBuffer[6 + inputReport.bluetooth] << '\n';
		std::cout << "Battery Level: " << inputReport.batteryLevel << "%\n";

	}
}

int initializeFakeController(XINPUT_STATE& ControllerState, PVIGEM_TARGET& emulateX360, VIGEM_ERROR& target, PVIGEM_CLIENT& client) {

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
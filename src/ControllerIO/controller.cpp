#include "controller.h"

Controller::Controller() {

	while (true) {
		hid_device_info* deviceInfo = hid_enumerate(SONY_VENDOR_ID, DUALSENSE_PRODUCT_ID);

		if (deviceInfo == nullptr) continue;

		Controller* x360Controller = new Dualsense;

		(*x360Controller).deviceHandle = CreateFileA(deviceInfo->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		(*x360Controller).bluetooth = deviceInfo->interface_number == -1;

		(*x360Controller).isConnected = true;
		hid_free_enumeration(deviceInfo);

		if ((*x360Controller).bluetooth) { //Bluetooth
			(*x360Controller).bufferSize = 78;
			(*x360Controller).outputSize = 547;
			(*x360Controller).inputBuffer[0] = 0x31; //Data report code
			controllerType = x360Controller;
		}
		//USB
		(*x360Controller).bufferSize = 64;
		(*x360Controller).outputSize = 64;
		(*x360Controller).inputBuffer[0] = 0x01; //Data report code
		controllerType = x360Controller;
	}
};// FUNCTION POINTER
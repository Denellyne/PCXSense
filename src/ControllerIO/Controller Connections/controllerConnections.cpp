#include "controllerConnections.h"

int initializeFakeController(PVIGEM_TARGET& emulateX360, VIGEM_ERROR& target, PVIGEM_CLIENT& client) {

	if (client == nullptr) return -1;

	const auto retval = vigem_connect(client);

	if (!VIGEM_SUCCESS(retval)) return -1;

	emulateX360 = vigem_target_x360_alloc();

	target = vigem_target_add(client, emulateX360);

	return 0;
}

bool isDualShoch4Connected(controller& x360Controller) {
	hid_device_info* deviceInfo = hid_enumerate(SONY_VENDOR_ID, DUALSHOCK4_PRODUCT_ID);

	if (deviceInfo == nullptr) return false;

	//deviceInfo->path different from other controller

	x360Controller.deviceHandle = CreateFileA(deviceInfo->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	x360Controller.bluetooth = deviceInfo->interface_number == -1;
	x360Controller.isConnected = true;
	hid_free_enumeration(deviceInfo);

	if (x360Controller.bluetooth) { //Bluetooth
		x360Controller.bufferSize = 547;
		x360Controller.inputBuffer[0] = 0x11; //Data report code
		return true;
	}
	//USB
	x360Controller.bufferSize = 64;
	x360Controller.inputBuffer[0] = 0x01; //Data report code
	return true;

}
bool isDualsenseConnected(controller& x360Controller){
	hid_device_info* deviceInfo = hid_enumerate(SONY_VENDOR_ID, DUALSENSE_PRODUCT_ID);

	if (deviceInfo == nullptr) return false;
	x360Controller.deviceHandle = CreateFileA(deviceInfo->path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	x360Controller.bluetooth = deviceInfo->interface_number == -1;
	x360Controller.isConnected = true;
	hid_free_enumeration(deviceInfo);

	if (x360Controller.bluetooth) { //Bluetooth
		x360Controller.bufferSize = 78;
		x360Controller.inputBuffer[0] = 0x31; //Data report code
		return true;
	}
	//USB
	x360Controller.bufferSize = 64;
	x360Controller.inputBuffer[0] = 0x01; //Data report code
	return true;

}
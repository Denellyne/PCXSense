#include "controllerConnections.h"
#include <mutex>

int initializeFakeController(PVIGEM_TARGET& emulateX360, VIGEM_ERROR& target, PVIGEM_CLIENT& client) {

	if (client == nullptr) return -1;

	const auto retval = vigem_connect(client);

	if (!VIGEM_SUCCESS(retval)) return -1;

	emulateX360 = vigem_target_x360_alloc();

	target = vigem_target_add(client, emulateX360);

	return 0;
}

bool isDualShock4Connected(controller& x360Controller) {
	x360Controller.deviceHandle = hid_open(SONY_VENDOR_ID, DUALSHOCK4_PRODUCT_ID, NULL);
	if (x360Controller.deviceHandle == nullptr) {
		printf("%ls\n", hid_error(x360Controller.deviceHandle));
		return false;
	}


	x360Controller.bluetooth = hid_get_device_info(x360Controller.deviceHandle)->interface_number == -1;
	x360Controller.isConnected = true;

	if (x360Controller.bluetooth) { //Bluetooth
		x360Controller.bluetooth = 0;
		x360Controller.bufferSize = 547;
		x360Controller.inputBuffer[0] = 0x11; //Data report code
		return true;
	}
	//USB
	x360Controller.bluetooth = 1; //Usb has a 2 offset from bluetooth
	x360Controller.bufferSize = 64;
	x360Controller.inputBuffer[0] = 0x01; //Data report code
	return true;

}
bool isDualsenseConnected(controller& x360Controller){

	x360Controller.deviceHandle = hid_open(SONY_VENDOR_ID, DUALSENSE_PRODUCT_ID, NULL);

	if (x360Controller.deviceHandle == nullptr) {
		printf("%ls\n",hid_error(x360Controller.deviceHandle));
		return false;
	}

	x360Controller.bluetooth = hid_get_device_info(x360Controller.deviceHandle)->interface_number == -1;
	x360Controller.isConnected = true;

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

bool isDualsenseEdgeConnected(controller& x360Controller) {

	x360Controller.deviceHandle = hid_open(SONY_VENDOR_ID, DUALSENSEEDGE_PRODUCT_ID, NULL);

	if (x360Controller.deviceHandle == nullptr) {
		printf("%ls\n", hid_error(x360Controller.deviceHandle));
		return false;
	}

	x360Controller.bluetooth = hid_get_device_info(x360Controller.deviceHandle)->interface_number == -1;

	x360Controller.isConnected = true;

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
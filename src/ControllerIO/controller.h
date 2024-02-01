#include <string>
#include <windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"
#include <hidapi.h>
#include <vector>
#include "Dualsense.h"

#define SONY_VENDOR_ID 0x054c

#define DUALSENSE_PRODUCT_ID 0x0ce6
#define DUALSHOCK4_PRODUCT_ID1 0x05C4 
#define DUALSHOCK4_PRODUCT_ID2 0x09CC 
#define DUALSHOCK4_PRODUCT_ID3 0x0BA0 
#define DUALSHOCK4_PRODUCT_ID4 0x0BA1


int initializeFakeController(PVIGEM_TARGET& emulateX360, VIGEM_ERROR& target, PVIGEM_CLIENT& client);

class Controller{
public:

	struct RGB {
		float colors[3]{}; //Red Green Blue
		short int microhponeLed;
		int Index;
	};

	unsigned char inputBuffer[78]{};
	PVIGEM_CLIENT client;
	PVIGEM_TARGET emulateX360;
	XINPUT_STATE ControllerState;
	short int batteryLevel;

	int bufferSize;
	short int outputSize{};

	bool bluetooth;
	int shortTriggers{};

	RGB RGB[10]{};
	bool isConnected;
	//bool rainbow{};

	HANDLE deviceHandle;
	VIGEM_ERROR target;

	Controller* controllerType{};

	Controller() = delete;
	Controller() = default;
	
};


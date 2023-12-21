#define DIRECTINPUT_VERSION 0x0800

#include <iostream>
#include <windows.h>
#include <DInput8.h>
#include <Xinput.h>


#ifdef NDEBUG
#define DEBUG(x) 
#else
#define DEBUG(x) do { std::cerr << x << std::endl; } while (0)
#endif
#ifdef _DEBUG
#include <chrono>
#include <iostream>
class Timer
{
public:
	Timer() {
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}
	~Timer() {
		Stop();
	}
	void Stop() {
		auto endTimePoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

		auto duration = end - start;
		double ms = duration * 0.001;

		std::cout << duration << "us ( " << ms << "ms )\n";
	}

private:
	std::chrono::time_point< std::chrono::high_resolution_clock> m_StartTimepoint;
};
#endif // _DEBUG



int main() {
	HINSTANCE appHandle = GetModuleHandle(NULL);
	IDirectInput8* ptrDirectInput = nullptr;

	if (DirectInput8Create(appHandle, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&ptrDirectInput, NULL) != DI_OK) {
		std::cout << "Failed to create DI8 Device\n";
		return -1;
	}
	LPDIRECTINPUTDEVICE8 controllerInterface;
	if (ptrDirectInput->CreateDevice(GUID_Joystick, &controllerInterface, NULL) != DI_OK) {
		ptrDirectInput->Release();
		std::cout << "Failed to create Device\n";
		return -1;
	}	
	if(controllerInterface->SetDataFormat(&c_dfDIJoystick2) != DI_OK){
		ptrDirectInput->Release();
		std::cout << "Failed to set Data Format\n";
		return -1;
	}
	if (controllerInterface->Acquire() != DI_OK) {
		ptrDirectInput->Release();
		std::cout << "Failed to acquire Device\n";
		return -1;
	}


	while (true) {
		
#ifdef _DEBUG
		//Timer a;
#endif // _DEBUG


		HRESULT result = controllerInterface->Poll();
		DIJOYSTATE2 js{};	
		if (controllerInterface->GetDeviceState(sizeof(DIJOYSTATE2), &js) != DI_OK) {
			ptrDirectInput->Release();
			std::cout << "Failed to get Device State\n";
			return -1;
		}
		result = controllerInterface->GetDeviceState(sizeof(DIJOYSTATE2), &js);

		if (js.rgbButtons[0]) DEBUG("Square Button\n");
		if (js.rgbButtons[1]) DEBUG("X Button\n");
		if (js.rgbButtons[2]) DEBUG("Circle Button\n");
		if (js.rgbButtons[3]) DEBUG("Triangle Button\n");
		if (js.rgbButtons[4]) DEBUG("L1 Button\n");
		if (js.rgbButtons[5]) DEBUG("R1 Button\n");
		if (js.rgbButtons[8]) DEBUG("Select Button\n");
		if (js.rgbButtons[9]) DEBUG("Start Button\n");
		if (js.rgbButtons[10]) DEBUG("L3 Button\n");
		if (js.rgbButtons[11]) DEBUG("R3 Button\n");
		if (js.rgbButtons[12]) DEBUG("Sony/Home Button\n");
		if (js.rgbButtons[13]) DEBUG("Toutchpad Click\n");
		if (js.rgbButtons[14]) DEBUG("Microphone Button\n");


		uint16_t rgdwPOV = 0;
		for (int i = 0; i < 4; i++) { // In banks of 4, shift in the sate of each DPAD 0-16 bits 
			switch (js.rgdwPOV[i]) {

			case 0:     DEBUG("Dpad Up\n"); break;
			case 31500:  DEBUG("Dpad Up and Dpad Left\n"); break;
			case 4500:  DEBUG("Dpad Up and Dpad  Right\n"); break;

			case 27000: DEBUG("Dpad Left\n"); break;
			case 9000:  DEBUG("Dpad right\n"); break;

			case 18000: DEBUG("Dpad Down\n"); break;

			case 22500:  DEBUG("Dpad Down and Dpad Left\n"); break;
			case 13500:  DEBUG("Dpad Down and Dpad Right\n"); break;
			}
		}

//		std::cout << "LeftJoystick Horizontal Value: " << js.lX / 257 << '\n';
//		std::cout << "LeftJoystick Vertical Value: " << js.lY / 257 << '\n';
//		std::cout << "RightJoystick Horizontal Value: " << js.lZ / 257 << '\n';
//		std::cout << "RightJoystick Horizontal Value: " << js.lRz / 257 << '\n';
//		std::cout << "Left Trigger Value: " << js.lRx / 257 << '\n';
//		std::cout << "Right Trigger Value: " << js.lRy / 257 << '\n';

	}

	ptrDirectInput->Release();

	return 0;
}
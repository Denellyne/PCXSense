#include "gyro.h"
#include <windows.h>
#include <thread>
#include <iostream>
extern bool gyroEnabled;
#define INPUTSIZE 20
unsigned char inputBuffer[INPUTSIZE]{ 0x05 };

class gyroCoords{
public:

	gyroCoords() {
		x = 0;
		y = 0;
	}
		
	int x, y;

	void getCoords(const controller& x360Controller) {
		if (hid_read(x360Controller.deviceHandle, inputBuffer, INPUTSIZE) == -1) {
			printf("YEAH");
			return;
		}
		if (!gyroEnabled) std::this_thread::sleep_for(std::chrono::milliseconds(10));
		system("cls");
		//inputBuffer[0] = 0x05;
		for (int i = 15; i <= 40; i += 2) {
			int z = 0;
			z = x360Controller.inputBuffer[i];
			z = z << 8;
			z |= x360Controller.inputBuffer[i + 1];
			std::cout << z << '\n';

		}
	//	for (int i = 0; i < INPUTSIZE; i++)
		//	std::cout << i << ": " << (int)inputBuffer[i] << ", ";
	}
};

void gyroController(const controller& x360Controller) {

	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
	const short sizeOfInput = sizeof(input);
	gyroCoords position;
	while (true) {
		if (!gyroEnabled) std::this_thread::sleep_for(std::chrono::milliseconds(500));
		else {
			position.getCoords(x360Controller);
			input.mi.dx = MulDiv(position.x, 65536, GetSystemMetrics(SM_CXSCREEN));
			input.mi.dy = MulDiv(position.y, 65536, GetSystemMetrics(SM_CYSCREEN));
			//SendInput(1, &input, sizeOfInput);
		}
	}
}
#pragma once
#include "controller.h"

extern "C" int returnSmaller(int x, int y);

class Dualsense : public Controller{
public:
	void inputReport(Controller* x360Controller);

	Dualsense() = default;

	

	~Dualsense() {
		vigem_target_remove(client, emulateX360);
		vigem_target_free(emulateX360);
		vigem_disconnect(client);
		vigem_free(client);
	}

private:
	void inline setButtons();
	void inline setButtonsGameProfile();

};


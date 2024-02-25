#pragma once

#include <tchar.h>
#include <fstream>
#include <filesystem>
#include "User Settings/Macros/macro.h"


extern unsigned char ptrCurrentTriggerProfile[8];
extern bool profileOpen;

class gameProfile {
public:
	std::string profileName{};
	std::string appNameLiteral{};
	std::wstring appName{};
	std::vector<Macros> gameMacros{};

	int buttonMapping[15]{ XINPUT_GAMEPAD_X ,XINPUT_GAMEPAD_A,XINPUT_GAMEPAD_B,XINPUT_GAMEPAD_Y,
						   XINPUT_GAMEPAD_LEFT_SHOULDER,XINPUT_GAMEPAD_RIGHT_SHOULDER,
						   XINPUT_GAMEPAD_BACK,XINPUT_GAMEPAD_START,
						   XINPUT_GAMEPAD_LEFT_THUMB,XINPUT_GAMEPAD_RIGHT_THUMB,
						   0,0,0,0,0 }; //First element of this line is treated as a boolean for DpadToJoystick and the other are Dualsense Edge Extras

	unsigned char gameTriggerProfile[8]{};
	bool rumbleTriggers = 0;
	RGB Lightbar{};

    bool inline isOpen();

};

void asyncGameProfile(std::vector<gameProfile>& gameProfiles, controller& x360Controller);
void loadProfiles(std::vector<gameProfile>& gameProfiles);
void saveProfiles(const std::vector<gameProfile> gameProfiles);
void profileEditor(bool& makerOpen, gameProfile& currentProfile, controller& x360Controller);
void profileMenu(bool& profileOpen, std::vector<gameProfile>& gameProfiles, controller& x360Controller);
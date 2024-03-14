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
	                       // Default Values
	int buttonMapping[20]{ XINPUT_GAMEPAD_X, // Square
	                       XINPUT_GAMEPAD_A, // Cross
	                       XINPUT_GAMEPAD_B, // Circle
	                       XINPUT_GAMEPAD_Y, // Triangle
	                       XINPUT_GAMEPAD_LEFT_SHOULDER, // L1
	                       XINPUT_GAMEPAD_RIGHT_SHOULDER, // R1
	                       XINPUT_GAMEPAD_BACK, // Share
	                       XINPUT_GAMEPAD_START, // Options
	                       XINPUT_GAMEPAD_LEFT_THUMB, // L3
	                       XINPUT_GAMEPAD_RIGHT_THUMB, // R3
	                       0, // DpadToJoystick (Bool)
	                       0, // Start/Select on Touchpad (Bool)
						   0, // Touchpad Button
						   0, // Home/Sony Button
						   0, // Mic Button
						   0, // Edge Only : Left Function
						   0, // Edge Only : Right Function
						   0, // Edge Only : Left Paddle
						   0, // Edge Only : Right Paddle
						   0, // Button Sensitive Rumble
	}; 

	short int rumbleButton[10]{ 0 };

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
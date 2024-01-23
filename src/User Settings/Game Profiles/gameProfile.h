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
	std::vector<Macros> gameMacros;
	unsigned char gameTriggerProfile[8]{};
  	RGB Lightbar;

    bool inline isOpen();

};

void asyncGameProfile(std::vector<gameProfile>& gameProfiles, controller& x360Controller);
void loadProfiles(std::vector<gameProfile>& gameProfiles);
void saveProfiles(const std::vector<gameProfile> gameProfiles);
void profileEditor(bool& makerOpen, gameProfile& currentProfile, controller& x360Controller);
void profileMenu(bool& profileOpen, std::vector<gameProfile>& gameProfiles, controller& x360Controller);
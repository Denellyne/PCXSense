#pragma once
#include "User Settings/Macros/macro.h"
#include "User Settings/Adaptive Triggers/Adaptive Triggers.h"
#include <tchar.h>
#include <fstream>
#include <filesystem>

extern unsigned char ptrCurrentTriggerProfile[8];
extern bool profileOpen;

class gameProfile {
public:
	std::string profileName{};
	std::string appNameLiteral{};
	std::wstring appName{};
	std::vector<Macros> gameMacros;
	unsigned char gameTriggerProfile[8]{};
  //  Lightbar Color;

    bool inline isOpen();


};

void asyncGameProfile(std::vector<gameProfile>& gameProfiles, const controller& x360Controller);
void loadProfiles(std::vector<gameProfile>& gameProfiles);
void saveProfiles(const std::vector<gameProfile> gameProfiles);
void profileEditor(bool& makerOpen, gameProfile& currentProfile, const controller& x360Controller);
void profileMenu(bool& profileOpen, std::vector<gameProfile>& gameProfiles, const controller& x360Controller);
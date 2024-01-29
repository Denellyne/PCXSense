#pragma once
#include "gameProfile.h"

void saveProfiles(const std::vector<gameProfile> gameProfiles);
void inline writeProfiles(std::string dirEntry, gameProfile& currentProfile);
void loadProfiles(std::vector<gameProfile>& gameProfiles);
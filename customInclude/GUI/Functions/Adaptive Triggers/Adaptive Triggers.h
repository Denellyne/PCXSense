#pragma once
#include "Misc/util.h"
extern bool triggerOpen;


struct triggerProfile {
	std::string name{};
	unsigned char trigger[8]{};
};

void triggerEditor(bool& makerOpen, triggerProfile& trigger);
void saveTrigger(const std::vector<triggerProfile> triggers);
void triggerMenu(std::vector<triggerProfile>& triggers);
void loadTrigger(std::vector<triggerProfile>& triggers);
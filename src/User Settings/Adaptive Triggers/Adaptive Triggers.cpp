#include "Adaptive Triggers.h"
#include "GUI\Functions\Misc\functionality.h"
#include <fstream>
#include <map>

static std::map<int, const char*> triggerModes = {
		{0x0,"Off"},
		{0x1,"Rigid"},
		{0x2,"Pulse"},
		{0x1 | 0x20,"Rigid Extra 1"},
		{0x1 | 0x04,"Rigid Extra 2"},
		{0x1 | 0x20 | 0x04,"Rigid Complete Mode"},
		{0x2 | 0x20,"Pulse Extra 1"},
		{0x2 | 0x04,"Pulse Extra 2"},
		{0x2 | 0x20 | 0x04,"Pulse Complete Mode"},
};

static const short int triggerModeKeys[9] = { 
												0x0, //Off
												0x1, //Rigid
												0x2, //Pulse
												0x1 | 0x20, //Rigid_A
												0x1 | 0x04 ,  //Rigid_B
												0x1 | 0x20 | 0x04 , //Rigid AB
												0x2 | 0x20, //Pulse_A
												0x2 | 0x04,  //Pulse_B
												0x2 | 0x20 | 0x04 }; //Pulse AB

extern unsigned char ptrCurrentTriggerProfile[8]{};
extern bool gameProfileSet;

void triggerToProfile(std::vector<gameProfile>& gameProfiles) {

	std::ifstream restoreProfile("triggers.txt");

	if (restoreProfile.is_open()) {
		char triggerName[128]{};
		int trigger[8]{};
		while (restoreProfile.good()) {
			gameProfile currentProfile{};
			ZeroMemory(triggerName, 0);


			restoreProfile.getline(triggerName, 128);
			for (short int i = 0; i < 8; i++) restoreProfile >> trigger[i];
			restoreProfile.get();

			currentProfile.profileName = triggerName;
			for (short int i = 0; i < 8; i++) currentProfile.gameTriggerProfile[i] = trigger[i];

			if (strcmp(triggerName,"No Profile") == 0) continue;

			gameProfiles.push_back(currentProfile);

		}
		restoreProfile.close();
		DeleteFile(L"triggers.txt");
		gameProfiles.pop_back();
		saveProfiles(gameProfiles);
	}

}

void triggerEditor(bool& makerOpen, unsigned char* trigger,bool& rumbleTriggers) {

	rumble[0] = 0;
	rumble[1] = 0;

	if (ImGui::Begin("Adaptive Trigger Maker", &makerOpen)) {
		static int motor1{}, motor2{};

		gameProfileSet = true;
		memcpy(&ptrCurrentTriggerProfile, &*trigger, 8);
		

		if (ImGui::BeginCombo("Trigger Mode", triggerModes[trigger[0]])) {
			for (short int i = 0; i < IM_ARRAYSIZE(triggerModeKeys); i++) if (ImGui::Selectable(triggerModes[triggerModeKeys[i]])) trigger[0] = triggerModeKeys[i];
			ImGui::EndCombo();
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Pulse Extra 2 adds option for Rumble Sensitive Triggers");
			ImGui::EndTooltip();
		}

		if (trigger[0] == (0x2 | 0x04)) {
			if (ImGui::RadioButton("Dinamic Rumble to Triggers", rumbleTriggers))
				rumbleTriggers = !rumbleTriggers;

			if (rumbleTriggers) {
				ZeroMemory(&trigger[3], 0);

				ImGui::SliderInt("##2", (int*)&trigger[1], 0, 15);
				ImGui::SliderInt("##3", (int*)&trigger[2], 0, 200);

				ImGui::Text("Rumble Edit");
				ImGui::SliderInt("##4", &motor1, 0, 255);
				ImGui::SliderInt("##5", &motor2, 0, 255);

				rumble[0] = motor1;
				rumble[1] = motor2;

				ptrCurrentTriggerProfile[1] += (int)rumble[0] >> 4;
				ptrCurrentTriggerProfile[2] += (int)rumble[1] >> 4;
				if ((int)rumble[0] >= 252) ptrCurrentTriggerProfile[3] = 220;


				ImGui::End();
				return;
			}
			
		}
				
		else
			rumbleTriggers = false;

		//Changed SliderInt Datatype so it displays the values properly
		ImGui::SliderInt("##2", (int*)&trigger[1], 0, 255);
		ImGui::SliderInt("##3", (int*)&trigger[2], 0, 255);
		ImGui::SliderInt("##4", (int*)&trigger[3], 0, 255);
		ImGui::SliderInt("##5", (int*)&trigger[4], 0, 255);
		ImGui::SliderInt("##6", (int*)&trigger[5], 0, 255);
		ImGui::SliderInt("##7", (int*)&trigger[6], 0, 255);
		ImGui::SliderInt("##8", (int*)&trigger[7], 0, 255);

	}
	ImGui::End();
}

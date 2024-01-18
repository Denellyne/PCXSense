#include "Adaptive Triggers.h"
#include "GUI\Functions\Misc\functionality.h"
#include <fstream>
extern unsigned char ptrCurrentTriggerProfile[8]{};


void saveTrigger(const std::vector<triggerProfile> triggers) {
	std::ofstream writeTriggers("triggers.txt");
	if (writeTriggers.is_open()) {
		for (triggerProfile triggers : triggers) {
			writeTriggers << triggers.name << '\n';
			for (short int i = 0; i < 8; i++) writeTriggers << (int)triggers.trigger[i] << '\n';
		}
		writeTriggers.close();
	}
}

void loadTrigger(std::vector<triggerProfile>& triggers){

	std::ifstream loadTrigger("triggers.txt");
	if (loadTrigger.is_open()) {
		char triggerName[128]{};
		int trigger[8]{};

		while (loadTrigger.good()) {
			triggerProfile currentTrigger{};
			ZeroMemory(triggerName, 0);

			loadTrigger.getline(triggerName, 128);
			for (short int i = 0; i < 8; i++) loadTrigger >> trigger[i];
			loadTrigger.get();

			currentTrigger.name = triggerName;
			for (short int i = 0; i < 8; i++) currentTrigger.trigger[i] = trigger[i];


			triggers.push_back(currentTrigger);
		}

		//Error correction
		triggers.erase(triggers.begin()+1);
		triggers.erase(triggers.begin()+1);
		triggers.pop_back();

		//Cleanup
		loadTrigger.close();
	}
}


void triggerEditor(bool& makerOpen, triggerProfile& editableTrigger) {

	static const short int triggerModes[9] = {  0, //Off
												1, //Rigid
												2, //Pulse
												0x20 | 0x1, //Rigid_A
												0x4 | 0x1,  //Rigid_B
												0x20 | 0x4 | 0x1, //Rigid AB
												0x20 | 0x2, //Pulse_A
												0x4 | 0x2,  //Pulse_B
												0x20 | 0x4 | 0x2, }; //Pulse AB

	if (ImGui::Begin("Adaptive Trigger Maker", &makerOpen)) {
		memcpy(&ptrCurrentTriggerProfile, &editableTrigger.trigger, 8);

		ImGui::InputText("Trigger Profile Name", &editableTrigger.name);
		if (ImGui::BeginCombo("Trigger Mode", std::format("{}", (int)editableTrigger.trigger[0]).c_str())) {
			for (short int i = 0; i < 9; i++) if (ImGui::Selectable(std::format("{}", triggerModes[i]).c_str())) editableTrigger.trigger[0] = triggerModes[i];
			ImGui::EndCombo();
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("0 = Off\n1 = Rigid\n2 = Pulse\n33 = Rigid_A\n5 = Rigid_B\n37 = Rigid_AB\n34 = Pulse_A\n6 = Pulse_B\n38 = Pulse_AB");
			ImGui::EndTooltip();
		}

		//Changed SliderInt Datatype so it displays the values properly
		ImGui::SliderInt("##2", (int*)&editableTrigger.trigger[1], 0, 255);
		ImGui::SliderInt("##3", (int*)&editableTrigger.trigger[2], 0, 255);
		ImGui::SliderInt("##4", (int*)&editableTrigger.trigger[3], 0, 255);
		ImGui::SliderInt("##5", (int*)&editableTrigger.trigger[4], 0, 255);
		ImGui::SliderInt("##6", (int*)&editableTrigger.trigger[5], 0, 255);
		ImGui::SliderInt("##7", (int*)&editableTrigger.trigger[6], 0, 255);
		ImGui::SliderInt("##8", (int*)&editableTrigger.trigger[7], 0, 255);
	}
	ImGui::End();


}

void triggerMenu(std::vector<triggerProfile>& triggers) {
	bool static makerOpen = false;

	static short int index{};
	if (makerOpen) (triggerEditor(makerOpen, triggers[index]));

	if (ImGui::Begin("Trigger Editor", &triggerOpen)) {
		for (short int i = 0; i < triggers.size(); i++) {
			ImGui::PushID(&triggers[i]);
			if (ImGui::Button(triggers[i].name.c_str())) {
				makerOpen = true;
				index = i;
			}
			ImGui::SameLine();
			if (ImGui::Button("Set profile")) {
				memcpy(&ptrCurrentTriggerProfile,&triggers[i].trigger, 8);
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Profile")) {
				makerOpen = false;
				triggers.erase(triggers.begin() + i);
			}
			ImGui::PopID();
		}
		if (ImGui::Button("Create new trigger Profile")) {
			makerOpen = true;
			triggerProfile newTrigger{};
			triggers.push_back(newTrigger);
			index = triggers.size() - 1;
		}
	}
	ImGui::End();
}
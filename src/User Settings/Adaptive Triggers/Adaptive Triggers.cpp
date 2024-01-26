#include "Adaptive Triggers.h"
#include "GUI\Functions\Misc\functionality.h"

extern unsigned char ptrCurrentTriggerProfile[8]{};
extern bool gameProfileSet;

void triggerEditor(bool& makerOpen, unsigned char* trigger,bool& rumbleTriggers) {

	rumble[0] = 0;
	rumble[1] = 0;

	static const short int triggerModes[9] = { 0x0, //Off
												0x1, //Rigid
												0x2, //Pulse
												0x1 | 0x20, //Rigid_A
												0x1 | 0x04 ,  //Rigid_B
												0x1 | 0x20 | 0x04 , //Rigid AB
												0x2 | 0x20, //Pulse_A
												0x2 | 0x04,  //Pulse_B
												0x2 | 0x20 | 0x04 }; //Pulse AB

	if (ImGui::Begin("Adaptive Trigger Maker", &makerOpen)) {
		static int motor1{}, motor2{};

		gameProfileSet = true;
		memcpy(&ptrCurrentTriggerProfile, &*trigger, 8);
		

		if (ImGui::BeginCombo("Trigger Mode", std::format("{}", (int)trigger[0]).c_str())) {
			for (short int i = 0; i < IM_ARRAYSIZE(triggerModes); i++) if (ImGui::Selectable(std::format("{}", triggerModes[i]).c_str())) trigger[0] = triggerModes[i];
			ImGui::EndCombo();
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("0 = Off\n1 = Rigid\n2 = Pulse\n33 = Rigid_A\n5 = Rigid_B\n37 = Rigid_AB\n34 = Pulse_A\n6 = Pulse_B - Rumble affects Triggers\n38 = Pulse_AB\n");
			ImGui::EndTooltip();
		}

		if (trigger[0] == (0x2 | 0x04)) {
			if (ImGui::RadioButton("Dinamic Rumble to Triggers", rumbleTriggers)) 
				rumbleTriggers = !rumbleTriggers;

			if (rumbleTriggers) {

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

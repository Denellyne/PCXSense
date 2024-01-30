#include "button Mappings.h"

void buttonMappingEditor(bool& makerOpen, int* buttonProfile) {
	extern int buttonMapping[11];
	extern bool gameProfileSet;



	std::tuple < std::string, int >buttons[10] = {
		{"Square", 0x4000},
		{"Cross", 0x1000},
		{"Circle", 0x2000},
		{"Triangle", 0x8000},
		{"Left Shoulder", 0x0100},
		{"Right Shoulder", 0x0200},
		{"Select", 0x0020},
		{"Start", 0x0010},
		{"Left Thumb", 0x0040},
		{"Right Thumb", 0x0080},
	};

	for (int i = 0; i < 11; i++) buttonMapping[i] = buttonProfile[i];
	gameProfileSet = true;

	if (ImGui::Begin("Button Remapper", &makerOpen)) {

		for (short int i = 0; i < IM_ARRAYSIZE(buttons); i++) {
			ImGui::PushID(&buttonProfile[i]);
			ImGui::Text(std::get<0>(buttons[i]).c_str());
			ImGui::SameLine();
			if(ImGui::BeginCombo("##Change Button", std::format("{}", buttonProfile[i]).c_str())){
				for (short int j = 0; j < IM_ARRAYSIZE(buttons); j++) if (ImGui::Selectable(std::format("{}", std::get<0>(buttons[j])).c_str())) buttonProfile[i] = std::get<1>(buttons[j]);
				ImGui::EndCombo();
			}
			ImGui::PopID();
		}
		if (ImGui::RadioButton("Map dpad to joysticks", buttonProfile[10]))
			buttonProfile[10] = 1 * (buttonProfile[10] <= 0);
	}
	ImGui::End();
}
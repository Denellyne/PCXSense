#include "button Mappings.h"
#include <map>
#include <format>

static std::map<int, const char*> xboxButtons = {
	{0x0000,"None"},
	{0x4000,"X"},
	{0x1000,"A"},
	{0x2000,"B"},
	{0x8000,"Y"},
	{0x0100,"Left Shoulder"},
	{0x0200,"Right Shoulder"},
	{0x0020,"Back"},
	{0x0010,"Start"},
	{0x0040,"Left Thumb"},
	{0x0080,"Right Thumb"},
};

static const char* extraButtons[7]{
	"Touchpad      ",
	"Home          ",
	"Mic           ",
	"Left Function ",
	"Right Function",
	"Left Paddle   ",
	"Right Paddle  ",
};

static const char* psButtons[10]{
	"Square        ",
	"Cross         ",
	"Circle        ",
	"Triangle      ",
	"L1            ",
	"R1            ",
	"Share         ",
	"Options       ",
	"L3            ",
	"R3            ",
};

static const int xboxButtonsKeys[11]{ 0x0000,0x4000,0x1000,0x2000,0x8000,0x0100,0x0200,0x0020,0x0010,0x0040,0x0080 };
#define sumProfileRumble rumbleButton[0] + rumbleButton[1] + rumbleButton[2] + rumbleButton[3] + rumbleButton[4] + rumbleButton[5] + rumbleButton[6] + rumbleButton[7] + rumbleButton[8] + rumbleButton[9]

void buttonMappingEditor(bool& makerOpen, int* buttonProfile,int* rumbleButton) {
	extern int buttonMapping[20];

	extern bool gameProfileSet;

	for (int i = 0; i < IM_ARRAYSIZE(buttonMapping); i++) buttonMapping[i] = buttonProfile[i];
	gameProfileSet = true;

	if (ImGui::Begin("Button Remapper", &makerOpen)) {

		for (short int i = 0; i < IM_ARRAYSIZE(psButtons); i++) {
			ImGui::PushID(&buttonProfile[i]);
			ImGui::Text(psButtons[i]);
			ImGui::SameLine();
			if (ImGui::BeginCombo("##Change Button", xboxButtons[buttonProfile[i]])) {
				for (short int j = 0; j < IM_ARRAYSIZE(xboxButtonsKeys); j++) if (ImGui::Selectable(xboxButtons[xboxButtonsKeys[j]])) buttonProfile[i] = xboxButtonsKeys[j];
				ImGui::EndCombo();
			}
			ImGui::PopID();
		}

		for (short int i = (bool)buttonProfile[11]; i < IM_ARRAYSIZE(extraButtons); i++) {
			ImGui::PushID(&buttonProfile[i + 15]);
			ImGui::Text(extraButtons[i]);
			ImGui::SameLine();
			if (ImGui::BeginCombo("##Change Dualsense Extras", xboxButtons[buttonProfile[i + 12]])) {
				for (short int j = 0; j < IM_ARRAYSIZE(xboxButtonsKeys); j++) if (ImGui::Selectable(xboxButtons[xboxButtonsKeys[j]])) buttonProfile[i + 12] = xboxButtonsKeys[j];
				ImGui::EndCombo();
			}
			ImGui::PopID();
		}

		if (ImGui::RadioButton("Map D-Pad to joysticks", buttonProfile[10]))
			buttonProfile[10] = 1 * (buttonProfile[10] <= 0);

		if (ImGui::RadioButton("Start/Select on Touchpad", buttonProfile[11]))
			buttonProfile[11] = 1 * (buttonProfile[11] <= 0);

		if (ImGui::RadioButton("Button Sensitive Rumble", buttonProfile[19]))
			buttonProfile[19] = 1 * (buttonProfile[19] <= 0);

		if (buttonMapping[19]) {
			ImGui::Text(std::format("Current value:{}", sumProfileRumble).c_str());
			for (short int i = 0; i < IM_ARRAYSIZE(psButtons); i++) {
				ImGui::PushID(&buttonProfile[i-1]);
				ImGui::SliderInt(psButtons[i], &rumbleButton[i], 0, 255);
				if (sumProfileRumble > 255) rumbleButton[i] = 255 - (sumProfileRumble - rumbleButton[i]);
				ImGui::PopID();
			}
		}


		buttonProfile[12] = buttonProfile[12] * (buttonProfile[11] <= 0); //If Start/Select on touch is activated then the binding on the touchbutton is resetted
	}
	ImGui::End();
}
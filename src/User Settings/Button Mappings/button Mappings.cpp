#include "button Mappings.h"
#include <map>

static std::map<int, const char*> xboxButtons = {
	{0x0000,"None"},
	{0x4000,"X"},
	{0x1000,"A"},
	{0x2000,"B"},
	{0x8000,"Y"},
	{0x0100,"LB"},
	{0x0200,"RB"},
	{0x0020,"Back"},
	{0x0010,"Start"},
	{0x0040,"LSB"},
	{0x0080,"RSB"},
};

static const char* dualsenseEdgeExtras[4]{
	"L Fn    ",
	"R Fn    ",
	"L Back  ",
	"R Back  ",
};

static const char* psButtons[13]{
	"Square  ",
	"Cross   ",
	"Circle  ",
	"Triangle",
	"L1      ",
	"R1      ",
	"Share   ",
	"Options ",
	"L3      ",
	"R3      ",
	"PS      ",
	"Touchpad",
	"Mic     ",
};

static const int xboxButtonsKeys[11]{ 0x0000,0x4000,0x1000,0x2000,0x8000,0x0100,0x0200,0x0020,0x0010,0x0040,0x0080 };

void buttonMappingEditor(bool& makerOpen, int* buttonProfile) {
	extern int buttonMapping[19];
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

		for (short int i = 0; i < IM_ARRAYSIZE(dualsenseEdgeExtras); i++) {
			ImGui::PushID(&buttonProfile[i + 15]);
			ImGui::Text(dualsenseEdgeExtras[i]);
			ImGui::SameLine();
			if (ImGui::BeginCombo("##Change Dualsense Extras", xboxButtons[buttonProfile[i + 15]])) {
				for (short int j = 0; j < IM_ARRAYSIZE(xboxButtonsKeys); j++) if (ImGui::Selectable(xboxButtons[xboxButtonsKeys[j]])) buttonProfile[i + 15] = xboxButtonsKeys[j];
				ImGui::EndCombo();
			}
			ImGui::PopID();
		}

		if (ImGui::RadioButton("Map D-Pad to joysticks", buttonProfile[13]))
			buttonProfile[13] = 1 * (buttonProfile[13] <= 0);

		if (ImGui::RadioButton("Start/Select on Touchpad", buttonProfile[14]))
			buttonProfile[14] = 1 * (buttonProfile[14] <= 0);
	}
	ImGui::End();
}
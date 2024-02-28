#include "Lightbar.h"
#include "GUI/include.h"
#include <fstream>

extern bool lightbarOpen;

void saveLightSettings(const controller& controller){

	std::ofstream writeLight("light.txt");
	if (writeLight.is_open()) {
		for(int i = 1;i < 10;i++){
			writeLight << controller.RGB[i].colors[0] << '\n';
			writeLight << controller.RGB[i].colors[1] << '\n';
			writeLight << controller.RGB[i].colors[2] << '\n';
			writeLight << controller.RGB[i].microhponeLed << '\n';
		}

		writeLight.close();
	}
}

void loadLightSettings(controller& controller) {

	std::ifstream loadLight("light.txt");
	if (loadLight.is_open()) {
		for (int i = 1; i < 10; i++) {
			loadLight >> controller.RGB[i].colors[0];
			loadLight >> controller.RGB[i].colors[1];
			loadLight >> controller.RGB[i].colors[2];
			loadLight >> controller.RGB[i].microhponeLed;
		}

		loadLight.close();
	}
}

void lightbarEditor(bool& lightbarOpen,RGB* RGB) {
	ImVec2 combo_pos = ImGui::GetCursorScreenPos();
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::SetCursorScreenPos(ImVec2(combo_pos.x + style.FramePadding.x, combo_pos.y));
	float iconSize = ImGui::GetTextLineHeightWithSpacing() - style.FramePadding.y;

	std::string modifier;

	const static std::string batteryLevel[] = {"0%%:  ","12%%: ","27%%: ","37%%: ","50%%: ",
											   "62%%: ","75%%: ","87%%: ","100%%:" };

	static bool isOpen[9]{};
	
	if (ImGui::Begin("RGB Editor", &lightbarOpen)) {
		
		for (int i = 1; i < 10; i++) {
			ImGui::PushID(&RGB[i]);
			switch (RGB[i].microhponeLed) { //Makes adding different modifiers easier
			case 1:
				modifier = "Fixed";
				break;
			case 2:
				modifier = "Pulsating";
				break;
			default:
				modifier = "Off";
				break;
			}


			ImGui::Text(batteryLevel[i-1].c_str());
			ImGui::SameLine();
			if (ImGui::ColorButton("Lightbar", { (float)(RGB[i].colors[0]),(float)(RGB[i].colors[1]),(float)(RGB[i].colors[2]),1}, 0, {iconSize,iconSize}))
				isOpen[i - 1] = !isOpen[i - 1];
			ImGui::SameLine();
			if (ImGui::BeginCombo("Modifier", modifier.c_str())) {
				if (ImGui::Selectable("None"))  RGB[i].microhponeLed = 0;
				if (ImGui::Selectable("Fixed"))  RGB[i].microhponeLed = 1;
				if (ImGui::Selectable("Pulsating"))  RGB[i].microhponeLed = 2;
				ImGui::EndCombo();
			}

			if (isOpen[i - 1]) {
				ImGui::PushItemWidth(250);
				ImGui::ColorPicker3(std::format("##RGB Picker{}", i).c_str(), &RGB[i].colors[0], ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoAlpha);
				RGB[0].Index = i;
			}

			ImGui::PopID();
		}
	}
	ImGui::End();

}

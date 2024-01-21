#include "macro.h"
#include "GUI\Functions\Misc\functionality.h"
#include <fstream>

void saveMacros(const std::vector<Macros> Macro) {
	std::ofstream writeMacros("macros.txt");
	if(writeMacros.is_open()){
		for (Macros macro : Macro) {
			writeMacros << macro.Name << '\n';
			writeMacros << macro.buttonCombination << '\n';
			writeMacros << macro.input[0].ki.wVk << '\n';
			writeMacros << (char)macro.input[1].ki.wVk << '\n';
		}
		writeMacros.close();
	}
}

void loadMacros(std::vector<Macros>& Macro){
	std::ifstream loadMacros("macros.txt");
	if (loadMacros.is_open()) {
		char macroName[128]{};
		int input1, buttonCombination;
		char input2;

		while (loadMacros.good()) {
			Macros currentMacro{};
			ZeroMemory(macroName, 0);

			loadMacros.getline(macroName, 128);
			loadMacros >> buttonCombination;
			loadMacros >> input1;
			loadMacros >> input2;
			loadMacros.get();

			currentMacro.Name = macroName;
			currentMacro.buttonCombination = buttonCombination;
			currentMacro.input[0].ki.wVk = input1;
			currentMacro.input[1].ki.wVk = input2;

			Macro.push_back(currentMacro);
		}
		if (Macro.size() <= 3) Macro.pop_back(); //Error correction
		loadMacros.close();
	}
}

void macroEditor(bool& makerOpen, Macros& macro, const controller& x360Controller) {

	static std::string modifier;

	switch (macro.input[0].ki.wVk) { //Makes adding different modifiers easier
	case VK_CONTROL:
		modifier = "Control";
		break;
	case VK_MENU:
		modifier = "Alt";
		break;
	default:
		modifier = "None";
		break;
	}

	if(ImGui::Begin("Macro Maker##", &makerOpen)){
		static int notificationTimer = 0;
		static bool notificationOpen = false;
		if (notificationTimer == 1000) notificationOpen = false;
		else notificationTimer++;

		ImGui::InputText("Macro name", &macro.Name);
		if (ImGui::BeginCombo("Key",std::format("{}",(char)macro.input[1].ki.wVk).c_str())) {
			for (int i = 0; i < 26; i++) {
				if (ImGui::Selectable(std::format("{}", alphabet[i]).c_str())) macro.input[1].ki.wVk = alphabet[i];
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("Modifier", modifier.c_str())) {
			if (ImGui::Selectable("Control"))  macro.input[0].ki.wVk = VK_CONTROL;
			if (ImGui::Selectable("Alt"))  macro.input[0].ki.wVk = VK_MENU;
			if (ImGui::Selectable("None"))  macro.input[0].ki.wVk = 0;
			ImGui::EndCombo();
		}
		
		ImGui::Text("Set button combination");
		ImGui::SameLine();
		if (ImGui::SmallButton("Set")) {
			Sleep(3000); // Give time for the user to input the commands
			macro.buttonCombination = x360Controller.ControllerState.Gamepad.wButtons;
			notificationOpen = true;
			notificationTimer = 0;
		}
		if (ImGui::BeginItemTooltip()) {
			if (!notificationOpen) ImGui::Text("Press the buttons on the controller and wait 3 seconds");
			else ImGui::Text("Done");
			ImGui::EndTooltip();
		}
	}
	ImGui::End();
}

void inline macroEditor(Macros& macro, bool& makerOpen, const controller& x360Controller) {
	static std::string modifier;

	switch (macro.input[0].ki.wVk) { //Makes adding different modifiers easier
	case VK_CONTROL:
		modifier = "Control";
		break;
	case VK_MENU:
		modifier = "Alt";
		break;
	default:
		modifier = "None";
		break;
	}

	if (ImGui::Begin("Macro Maker##Non Overloaded", &makerOpen)) {
		static int notificationTimer = 0;
		static bool notificationOpen = false;
		if (notificationTimer == 1000) notificationOpen = false;
		else notificationTimer++;

		ImGui::InputText("Macro name", &macro.Name);
		if (ImGui::BeginCombo("Key", std::format("{}", (char)macro.input[1].ki.wVk).c_str())) {
			for (int i = 0; i < 26; i++) {
				if (ImGui::Selectable(std::format("{}", alphabet[i]).c_str())) macro.input[1].ki.wVk = alphabet[i];
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("Modifier", modifier.c_str())) {
			if (ImGui::Selectable("Control"))  macro.input[0].ki.wVk = VK_CONTROL;
			if (ImGui::Selectable("Alt"))  macro.input[0].ki.wVk = VK_MENU;
			if (ImGui::Selectable("None"))  macro.input[0].ki.wVk = 0;
			ImGui::EndCombo();
		}

		ImGui::Text("Set button combination");
		ImGui::SameLine();
		if (ImGui::SmallButton("Set")) {
			Sleep(3000); // Give time for the user to input the commands
			macro.buttonCombination = x360Controller.ControllerState.Gamepad.wButtons;
			notificationOpen = true;
			notificationTimer = 0;
		}
		if (ImGui::BeginItemTooltip()) {
			if (!notificationOpen) ImGui::Text("Press the buttons on the controller and wait 3 seconds");
			else ImGui::Text("Done");
			ImGui::EndTooltip();
		}
	}
	ImGui::End();
}

void macroMenu(std::vector<Macros>& Macro, const controller& x360Controller) {
	bool static makerOpenOverload = false;
	static short int index{};
	if (makerOpenOverload) (macroEditor(Macro[index], makerOpenOverload, x360Controller));

	if (ImGui::Begin("Macro Editor", &macroOpen)) {
		for (short int i = 0; i < Macro.size(); i++) {
			ImGui::PushID(&Macro[i]);
			if (ImGui::Button(Macro[i].Name.c_str())) {
				makerOpenOverload = true;
				index = i;
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Macro")) {
				makerOpenOverload = false;
				Macro.erase(Macro.begin() + i);
			}
			ImGui::PopID();
		}
		if (ImGui::Button("Create new Macro")) {
			makerOpenOverload = true;
			Macros newMacro{};
			Macro.push_back(newMacro);
			index = Macro.size() - 1;
		}
	}
	ImGui::End();
}

void macroMenu(bool& profileMacroOpen,std::vector<Macros>& Macro, const controller& x360Controller) {
	bool static makerOpen = false;
	static short int index{};
	if (makerOpen) (macroEditor(makerOpen, Macro[index], x360Controller));

	if (ImGui::Begin("Game Profile Macro Editor", &profileMacroOpen)) {
		for (short int i = 0; i < Macro.size(); i++) {
			ImGui::PushID(&Macro[i]);
			if (ImGui::Button(Macro[i].Name.c_str())) {
				makerOpen = true;
				index = i;
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Macro")) {
				makerOpen = false;
				Macro.erase(Macro.begin() + i);
			}
			ImGui::PopID();
		}
		if (ImGui::Button("Create new Macro")) {
			makerOpen = true;
			Macros newMacro{};
			Macro.push_back(newMacro);
			index = Macro.size() - 1;
		}
	}
	ImGui::End();
}

void asyncMacro(const controller& x360Controller, std::vector<Macros>& Macro) {

	while (true) {
		Sleep(20);
		for (Macros macro : Macro) {
			if (macro.buttonCombination == x360Controller.ControllerState.Gamepad.wButtons) {
				SendInput(ARRAYSIZE(macro.input), macro.input, sizeof(INPUT));
				macro.input[0].ki.dwFlags = KEYEVENTF_KEYUP;
				macro.input[1].ki.dwFlags = KEYEVENTF_KEYUP;
				SendInput(ARRAYSIZE(macro.input), macro.input, sizeof(INPUT));
				macro.input[0].ki.dwFlags = 0;
				macro.input[1].ki.dwFlags = 0;
				Sleep(1000);
			}
		}
	}

}
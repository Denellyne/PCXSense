#include "gameProfile.h"
#include <format>
#include <string>
#include <thread>
#include <iostream>
#include "GUI\Functions\Misc\functionality.h"
#include "User Settings/Macros/macro.h"
#include "User Settings/Adaptive Triggers/Adaptive Triggers.h"
#include "User Settings/Button Mappings/button Mappings.h"
#include "User Settings/Button Mappings/button Mappings.h"

extern bool gameProfileSet = false;
extern bool profileEdit = false;
extern UCHAR rumble[2];
extern UCHAR profileRumble{};
extern int buttonMapping[20]{};
extern int buttonRumble[10]{};
extern bool triggerMaker{ false }, profileMacroOpen{ false }, lightEditor{ false }, buttonRemapper{ false };
extern bool tempRumbleEnabled{ false };

#define sumProfileRumble gameProfiles[i].rumbleButton[0] + gameProfiles[i].rumbleButton[1] + gameProfiles[i].rumbleButton[2] + gameProfiles[i].rumbleButton[3] + gameProfiles[i].rumbleButton[4] + gameProfiles[i].rumbleButton[5] + gameProfiles[i].rumbleButton[6] + gameProfiles[i].rumbleButton[7] + gameProfiles[i].rumbleButton[8] + gameProfiles[i].rumbleButton[9]

BOOL inline static CALLBACK FindWindowBySubstr(HWND hwnd, LPARAM substring){
	const DWORD TITLE_SIZE = 1024;
	TCHAR windowTitle[TITLE_SIZE];

	if (GetWindowText(hwnd, windowTitle, TITLE_SIZE))
		if (_tcsstr(windowTitle, LPCTSTR(substring)) != NULL)
			return true;

	return false;
}

bool inline gameProfile::isOpen() {
	if (appName.length() == 0) return false;
	HWND foregroundWindow = GetForegroundWindow();
	
	return FindWindowBySubstr(foregroundWindow, (LPARAM)appName.c_str());
}

void inline static getRumble(const controller& x360Controller,const int* rumbleButtons) {

	profileRumble = (bool)(x360Controller.inputBuffer[8 + x360Controller.hidOffset] & (1 << 4)) ? rumbleButtons[0] : 0; //Square

	profileRumble += (bool)(x360Controller.inputBuffer[8 + x360Controller.hidOffset] & (1 << 5)) ? rumbleButtons[1] : 0; //Cross

	profileRumble += (bool)(x360Controller.inputBuffer[8 + x360Controller.hidOffset] & (1 << 6)) ? rumbleButtons[2] : 0; //Circle

	profileRumble += (bool)(x360Controller.inputBuffer[8 + x360Controller.hidOffset] & (1 << 7)) ? rumbleButtons[3] : 0; //Triangle

	profileRumble += (bool)(x360Controller.inputBuffer[9 + x360Controller.hidOffset] & (1 << 0)) ? rumbleButtons[4] : 0; //Left Shoulder

	profileRumble += (bool)(x360Controller.inputBuffer[9 + x360Controller.hidOffset] & (1 << 1)) ? rumbleButtons[5] : 0; //Right Shoulder

	profileRumble += (bool)(x360Controller.inputBuffer[9 + x360Controller.hidOffset] & (1 << 4)) ? rumbleButtons[6] : 0; //Select

	profileRumble += (bool)(x360Controller.inputBuffer[9 + x360Controller.hidOffset] & (1 << 5)) ? rumbleButtons[7] : 0; //Start

	profileRumble += (bool)(x360Controller.inputBuffer[9 + x360Controller.hidOffset] & (1 << 6)) ? rumbleButtons[8] : 0; //Left Thumb

	profileRumble += (bool)(x360Controller.inputBuffer[9 + x360Controller.hidOffset] & (1 << 7)) ? rumbleButtons[9] : 0; //Right thumb
	
}

void inline static asyncCheckMacro(const std::vector<Macros>& gameMacros, const controller& x360Controller) {

	while (gameProfileSet) {
		Sleep(20);
		for (Macros macro : gameMacros)
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

void inline static setProfile(controller& x360Controller,const gameProfile& gameProfile) {
	extern bool rumbleEnabled;

	if (gameProfile.buttonMapping[19]) {
		std::thread(asyncCheckMacro, std::ref(gameProfile.gameMacros), std::ref(x360Controller)).detach();
		rumbleEnabled = false;
	}
	gameProfileSet = true;

	//Set profile
	memcpy(&ptrCurrentTriggerProfile, &gameProfile.gameTriggerProfile, 8);
	for (int j = 0; j < ARRAYSIZE(buttonMapping); j++) buttonMapping[j] = gameProfile.buttonMapping[j];

	x360Controller.RGB[0].Index = 0;
	x360Controller.RGB[0].colors[0] = gameProfile.Lightbar.colors[0];
	x360Controller.RGB[0].colors[1] = gameProfile.Lightbar.colors[1];
	x360Controller.RGB[0].colors[2] = gameProfile.Lightbar.colors[2];
}

void asyncGameProfile(std::vector<gameProfile>& gameProfiles, controller& x360Controller) {
	extern bool rumbleEnabled;

	while (true) {
		while (profileEdit) Sleep(500);
		Sleep(500);

		for (int i = 0; i < gameProfiles.size();i++) {
			if (gameProfiles[i].isOpen()) {
				bool tempRumbleEnabled = rumbleEnabled;
				setProfile(x360Controller, gameProfiles[i]);

				while (gameProfiles[i].isOpen()) {
					getRumble(x360Controller, gameProfiles[i].rumbleButton);

					if (gameProfiles[i].rumbleTriggers) {
						memcpy(&ptrCurrentTriggerProfile, &gameProfiles[i].gameTriggerProfile, 8);
						ptrCurrentTriggerProfile[1] += ((rumble[0] >> 4) * rumbleEnabled + (bool)buttonMapping[19] * (profileRumble >> 4));
						ptrCurrentTriggerProfile[2] += ((rumble[1] >> 4) * rumbleEnabled + (bool)buttonMapping[19] * (profileRumble >> 4));
						if ((int)rumble[0] >= 230 || (int)profileRumble >= 230) ptrCurrentTriggerProfile[3] = 220; //Safeguard so triggers don't rattle to much damaging them
					}
					Sleep(5);
				}
			}
			profileRumble = 0;
			rumbleEnabled = tempRumbleEnabled;
			gameProfileSet = false;
		}
	}
}

void profileEditor(bool& profileEdit,gameProfile& currentProfile, controller& x360Controller){

	ImVec2 combo_pos = ImGui::GetCursorScreenPos();
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::SetCursorScreenPos(ImVec2(combo_pos.x + style.FramePadding.x, combo_pos.y));
	float iconSize = ImGui::GetTextLineHeightWithSpacing() - style.FramePadding.y;

	if (triggerMaker) (triggerEditor(triggerMaker, currentProfile.gameTriggerProfile,currentProfile.rumbleTriggers));
	if (profileMacroOpen) (macroMenu(profileMacroOpen,currentProfile.gameMacros, x360Controller));
	if (buttonRemapper) (buttonMappingEditor(buttonRemapper,currentProfile.buttonMapping,currentProfile.rumbleButton));


	if(ImGui::Begin("Profile Editor",&profileEdit)) {
		x360Controller.RGB[0].Index = 0;
		x360Controller.RGB[0].colors[0] = currentProfile.Lightbar.colors[0];
		x360Controller.RGB[0].colors[1] = currentProfile.Lightbar.colors[1];
		x360Controller.RGB[0].colors[2] = currentProfile.Lightbar.colors[2];

		ImGui::InputText("Profile Name", &currentProfile.profileName);
		ImGui::InputText("Application Name", &currentProfile.appNameLiteral);
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("You don't need to put the whole name of the app but a substring of it,example Yuzu app name is -> yuzu xxxx, you only need to put yuzu and beware its caps sensitive");
			ImGui::EndTooltip();
		}

		if (ImGui::Button("Edit Trigger Profile")) triggerMaker = true;
		if (ImGui::Button("Edit Macro Profile")) profileMacroOpen = true;
		if (ImGui::Button("Edit Button Mapping")) buttonRemapper = true;
		if (ImGui::ColorButton("Lightbar", { (float)(currentProfile.Lightbar.colors[0]),(float)(currentProfile.Lightbar.colors[1]),(float)(currentProfile.Lightbar.colors[2]),1 }, 0, { iconSize,iconSize }))
			lightEditor = !lightEditor;

		if (lightEditor) {
			ImGui::PushItemWidth(150);
			ImGui::SameLine();
			ImGui::ColorPicker3("##RGB Editor GameProfile", &currentProfile.Lightbar.colors[0], ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoAlpha);
		}
			

	}
	ImGui::End();
	currentProfile.appName = std::wstring(&currentProfile.appNameLiteral[0], &currentProfile.appNameLiteral[currentProfile.appNameLiteral.length()]);

}

void profileMenu(bool& profileOpen, std::vector<gameProfile>& gameProfiles, controller& x360Controller) {

	static int index{};

	if (profileEdit) profileEditor(profileEdit,gameProfiles[index], x360Controller);

	if (ImGui::Begin("Game Profile Menu", &profileOpen)) {
		for (short int i = 0; i < gameProfiles.size(); i++) {
			ImGui::PushID(&gameProfiles[i]);
			if (ImGui::Button(gameProfiles[i].profileName.c_str())) {
				profileEdit = true;
				index = i;
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Profile")) {
				profileEdit = false;
				std::filesystem::remove_all(std::format("Game Profiles/{}", gameProfiles[i].profileName).c_str());
				gameProfiles.erase(gameProfiles.begin() + i);
			}
			ImGui::PopID();
		}
		if (ImGui::Button("Create new Game Profile")) {
			profileEdit = true;
			gameProfile newProfile{};
			gameProfiles.push_back(newProfile);
			index = gameProfiles.size() - 1;
		}

	}
	ImGui::End();
}
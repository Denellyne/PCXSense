#include "gameProfile.h"
#include <format>
#include <string>
#include "GUI\Functions\Misc\functionality.h"
#include "User Settings/Macros/macro.h"
#include "User Settings/Adaptive Triggers/Adaptive Triggers.h"
#include <iostream>

extern bool gameProfileSet = false;
extern bool profileEdit = false;
extern UCHAR rumble[2];
extern int buttonMapping[11]{};

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

void inline checkMacro(Macros& macro, const controller& x360Controller) {

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

void asyncGameProfile(std::vector<gameProfile>& gameProfiles, controller& x360Controller) {

	while (true) {
		while (profileEdit) Sleep(500);

		Sleep(500);

		for (int i = 0; i < gameProfiles.size();i++) {
			while (gameProfiles[i].isOpen()) {
				//Set profile
				memcpy(&ptrCurrentTriggerProfile, &gameProfiles[i].gameTriggerProfile, 8);
				for (int j = 0; j < 11; j++) buttonMapping[j] = gameProfiles[i].buttonMapping[j];
				gameProfileSet = true;

				x360Controller.RGB[0].Index = 0;
				x360Controller.RGB[0].colors[0] = gameProfiles[i].Lightbar.colors[0];
				x360Controller.RGB[0].colors[1] = gameProfiles[i].Lightbar.colors[1];
				x360Controller.RGB[0].colors[2] = gameProfiles[i].Lightbar.colors[2];

				if (gameProfiles[i].rumbleTriggers) {
					ptrCurrentTriggerProfile[1] += (int)rumble[0] >> 4;
					ptrCurrentTriggerProfile[2] += (int)rumble[1] >> 4;
					if ((int)rumble[0] >= 230) ptrCurrentTriggerProfile[3] = 220; //Safeguard so triggers don't rattle to much damaging them
				}
				
				Sleep(20);
				for (Macros macro : gameProfiles[i].gameMacros) checkMacro(macro, x360Controller);

			}
			gameProfileSet = false;
		}
	}
}

void profileEditor(bool& profileEdit,gameProfile& currentProfile, controller& x360Controller){

	ImVec2 combo_pos = ImGui::GetCursorScreenPos();
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::SetCursorScreenPos(ImVec2(combo_pos.x + style.FramePadding.x, combo_pos.y));
	float iconSize = ImGui::GetTextLineHeightWithSpacing() - style.FramePadding.y;

	static bool triggerMaker{ false }, profileMacroOpen{ false }, lightEditor{false};
	if (triggerMaker) (triggerEditor(triggerMaker, currentProfile.gameTriggerProfile,currentProfile.rumbleTriggers));
	if (profileMacroOpen) (macroMenu(profileMacroOpen,currentProfile.gameMacros, x360Controller));


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
		if (ImGui::Button("Create new trigger Profile")) {
			profileEdit = true;
			gameProfile newProfile{};
			gameProfiles.push_back(newProfile);
			index = gameProfiles.size() - 1;
		}

	}
	ImGui::End();
}
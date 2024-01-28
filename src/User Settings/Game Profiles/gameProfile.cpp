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

BOOL inline CALLBACK FindWindowBySubstr(HWND hwnd, LPARAM substring){
	const DWORD TITLE_SIZE = 1024;
	TCHAR windowTitle[TITLE_SIZE];

	if (GetWindowText(hwnd, windowTitle, TITLE_SIZE))
		if (_tcsstr(windowTitle, LPCTSTR(substring)) != NULL)
			return false;

	return true;
}
bool inline gameProfile::isOpen() {
	if (appName.length() == 0) return false;
	if (EnumWindows(FindWindowBySubstr, (LPARAM)appName.c_str()) == false) return true;
	return false;
}

void saveProfiles(const std::vector<gameProfile> gameProfiles) {
	std::filesystem::remove_all("Game Profiles");
	std::filesystem::create_directory("Game Profiles");
	for (gameProfile profile : gameProfiles) {

		if (strlen(profile.profileName.c_str()) == 0) profile.profileName = "NULL";
		if (profile.appName.length() == 0) profile.appName = L"NULL";

		std::filesystem::create_directory(std::format("Game Profiles/{}",profile.profileName.c_str()));

		//Write Profile Strings and Trigger Profile and Lightbar Profile

		std::ofstream saveProfile(std::format("Game Profiles/{}/profile.txt",profile.profileName.c_str()));
		if (saveProfile.is_open()) {

			saveProfile << profile.profileName.c_str() << '\n';
			for(short int i = 0;i < profile.appName.length();i++) saveProfile << (char*)(&profile.appName[i]);
			saveProfile << '\n';

			//Write triggers
			for (short int i = 0; i < 8; i++) saveProfile << (int)profile.gameTriggerProfile[i] << '\n';

			saveProfile << profile.rumbleTriggers << '\n';
			
			saveProfile << profile.Lightbar.colors[0] << '\n';
			saveProfile << profile.Lightbar.colors[1] << '\n';
			saveProfile << profile.Lightbar.colors[2] << '\n';
			saveProfile << profile.Lightbar.microhponeLed << '\n';

			saveProfile.close();
		}

		//Write Macro Vector

		std::ofstream saveMacro(std::format("Game Profiles/{}/macros.txt", profile.profileName.c_str()));
		if (saveMacro.is_open()) {
			for (Macros macro : profile.gameMacros) {
				if (strlen(macro.Name.c_str()) == 0) macro.Name = "NULL";
				saveMacro << macro.Name.c_str() << '\n';
				saveMacro << macro.buttonCombination << '\n';
				saveMacro << macro.input[0].ki.wVk << '\n';
				saveMacro << (char)macro.input[1].ki.wVk << '\n';
			}
			saveMacro.close();
		}

	}
}


void inline writeProfiles(std::string dirEntry, gameProfile& currentProfile) {

	//Write strings and TriggerProfile

	std::ifstream writeStrings(std::format("{}/profile.txt", dirEntry));
	if (writeStrings.is_open()) {
		std::string profileName{};
		std::string appName{};
		int trigger[9]{};
		RGB Lightbar{};

		while (writeStrings.good()) {
			std::getline(writeStrings, profileName,'\n');
			memcpy(&currentProfile.profileName[0], &profileName[0], profileName.length());

			//It just works if I add this idk

			writeStrings.ignore();
			writeStrings.unget();
			//
			std::getline(writeStrings, appName,'\n');

			currentProfile.appName = std::wstring(&appName[0], &appName[appName.length()]);
			currentProfile.appNameLiteral = appName;


			for (short int i = 0; i < 9; i++) writeStrings >> trigger[i];

			for (short int i = 0; i < 8; i++) currentProfile.gameTriggerProfile[i] = trigger[i];
			currentProfile.rumbleTriggers = (bool)trigger[8];

			writeStrings >> Lightbar.colors[0];
			writeStrings >> Lightbar.colors[1];
			writeStrings >> Lightbar.colors[2];
			writeStrings >> Lightbar.microhponeLed;

			currentProfile.Lightbar.colors[0] = Lightbar.colors[0];
			currentProfile.Lightbar.colors[1] = Lightbar.colors[1];
			currentProfile.Lightbar.colors[2] = Lightbar.colors[2];
			currentProfile.Lightbar.microhponeLed = Lightbar.microhponeLed;

		}
		writeStrings.close();
	}

	//Write Macros to vector

	std::ifstream loadMacros(std::format("{}/macros.txt", dirEntry));
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

			currentProfile.gameMacros.push_back(currentMacro);
		}
		if (currentProfile.gameMacros.size() <= 3) currentProfile.gameMacros.pop_back(); //Error correction
		loadMacros.close();
	}

}

void loadProfiles(std::vector<gameProfile>& gameProfiles) {

	namespace fs = std::filesystem;

	if (!fs::is_directory(fs::path(".//Game Profiles"))) return;

	for (const auto& dirEntry : fs::directory_iterator(".//Game Profiles")) {
		gameProfile currentProfile;
		writeProfiles(dirEntry.path().string(), currentProfile);
		gameProfiles.push_back(currentProfile);
	}
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
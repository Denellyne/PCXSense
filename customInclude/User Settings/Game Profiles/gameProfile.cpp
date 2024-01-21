#include "gameProfile.h"
#include <format>
#include <string>
#include "GUI\Functions\Misc\functionality.h"

extern bool gameProfileSet = false;

BOOL inline CALLBACK FindWindowBySubstr(HWND hwnd, LPARAM substring){
	const DWORD TITLE_SIZE = 1024;
	TCHAR windowTitle[TITLE_SIZE];

	if (GetWindowText(hwnd, windowTitle, TITLE_SIZE))
		if (_tcsstr(windowTitle, LPCTSTR(substring)) != NULL)
			return false;

	return true;
}

bool inline gameProfile::isOpen() {
	if (EnumWindows(FindWindowBySubstr, (LPARAM)appName.c_str()) == false) return true;
	return false;
}

void saveProfiles(const std::vector<gameProfile> gameProfiles) {
	std::filesystem::create_directory("Game Profiles");
	for (gameProfile profile : gameProfiles) {
		std::filesystem::create_directory(std::format("Game Profiles/{}",profile.profileName));

		//Write Profile Strings and Trigger Profile

		std::ofstream writeProfile(std::format("Game Profiles/{}/profile.txt",profile.profileName));
		if (writeProfile.is_open()) {

			writeProfile << profile.profileName << '\n';
			for(short int i = 0;i < profile.appName.length();i++) writeProfile << (char*)(&profile.appName[i]);
			writeProfile << '\n';

			//Write triggers
			for (short int i = 0; i < 8; i++) writeProfile << (int)profile.gameTriggerProfile[i] << '\n';

			writeProfile.close();
		}

		//Write Macro Vector

		std::ofstream writeMacros(std::format("Game Profiles/{}/macros.txt", profile.profileName));
		if (writeMacros.is_open()) {
			for (Macros macro : profile.gameMacros) {
				writeMacros << macro.Name << '\n';
				writeMacros << macro.buttonCombination << '\n';
				writeMacros << macro.input[0].ki.wVk << '\n';
				writeMacros << (char)macro.input[1].ki.wVk << '\n';
			}
			writeMacros.close();
		}

	}
}

void inline writeProfiles(std::string dirEntry, gameProfile& currentProfile) {

	//Write strings and TriggerProfile

	std::ifstream writeStrings(std::format("{}/profile.txt", dirEntry));
	if (writeStrings.is_open()) {
		std::string profileName{};
		std::string appName{};
		int trigger[8]{};

		while (writeStrings.good()) {

			writeStrings >> profileName;
			writeStrings.get();
			currentProfile.profileName = profileName;

			writeStrings >> appName;
			writeStrings.get();
			currentProfile.appName = std::wstring(&appName[0], &appName[appName.length()]);
			currentProfile.appNameLiteral = appName;


			for (short int i = 0; i < 8; i++) writeStrings >> trigger[i];

			for (short int i = 0; i < 8; i++) currentProfile.gameTriggerProfile[i] = trigger[i];
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

	if (!fs::exists("Game Profiles")) return;

	for (const auto& dirEntry : fs::directory_iterator("Game Profiles")) {
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

void asyncGameProfile(std::vector<gameProfile>& gameProfiles, const controller& x360Controller){
	
	while (true) {
		
		Sleep(500);
		for (gameProfile profile : gameProfiles) {
			while (profile.isOpen()) {
				memcpy(&ptrCurrentTriggerProfile, &profile.gameTriggerProfile, 8);
				gameProfileSet = true;
				Sleep(20);
				for (Macros macro : profile.gameMacros) checkMacro(macro, x360Controller);
			}
			gameProfileSet = false;
		}
	}
}

void profileEditor(bool& makerOpen,gameProfile& currentProfile, const controller& x360Controller){

	static bool triggerMaker{ false }, profileMacroOpen{ false };
	if (triggerMaker) (triggerEditor(triggerMaker, currentProfile.gameTriggerProfile));
	if (profileMacroOpen) (macroMenu(profileMacroOpen,currentProfile.gameMacros, x360Controller));


	if(ImGui::Begin("Profile Editor",&makerOpen)) {

		ImGui::InputText("Profile Name", &currentProfile.profileName);
		ImGui::InputText("Application Name", &currentProfile.appNameLiteral);
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("You don't need to put the whole name of the app but a substring of it,example Yuzu app name is -> yuzu xxxx, you only need to put yuzu and beware its caps sensitive");
			ImGui::EndTooltip();
		}

		if (ImGui::Button("Edit Trigger Profile")) triggerMaker = true;
		if (ImGui::Button("Edit Macro Profile")) profileMacroOpen = true;

		ImGui::End();
	}
	currentProfile.appName = std::wstring(&currentProfile.appNameLiteral[0], &currentProfile.appNameLiteral[currentProfile.appNameLiteral.length()]);

}

void profileMenu(bool& profileOpen, std::vector<gameProfile>& gameProfiles, const controller& x360Controller) {

	static bool makerOpen = false;
	static int index{};

	if (makerOpen) profileEditor(makerOpen,gameProfiles[index], x360Controller);

	if (ImGui::Begin("Game Profile Menu", &profileOpen)) {
		for (short int i = 0; i < gameProfiles.size(); i++) {
			ImGui::PushID(&gameProfiles[i]);
			if (ImGui::Button(gameProfiles[i].profileName.c_str())) {
				makerOpen = true;
				index = i;
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Profile")) {
				makerOpen = false;
				std::filesystem::remove_all(std::format("Game Profiles/{}", gameProfiles[i].profileName).c_str());
				gameProfiles.erase(gameProfiles.begin() + i);
			}
			ImGui::PopID();
		}
		if (ImGui::Button("Create new trigger Profile")) {
			makerOpen = true;
			gameProfile newProfile{};
			gameProfiles.push_back(newProfile);
			index = gameProfiles.size() - 1;
		}

	}
	ImGui::End();
}
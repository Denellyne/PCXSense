#include "saveLoad.h"

void saveProfiles(const std::vector<gameProfile> gameProfiles) {
	std::filesystem::remove_all("Game Profiles");
	std::filesystem::create_directory("Game Profiles");
	for (gameProfile profile : gameProfiles) {

		if (strlen(profile.profileName.c_str()) == 0) profile.profileName = "NULL";
		if (profile.appName.length() == 0) profile.appName = L"NULL";

		std::filesystem::create_directory(std::format("Game Profiles/{}", profile.profileName.c_str()));

		//Write Profile Strings and Trigger Profile and Lightbar Profile

		std::ofstream saveProfile(std::format("Game Profiles/{}/profile.txt", profile.profileName.c_str()));
		if (saveProfile.is_open()) {

			saveProfile << profile.profileName.c_str() << '\n';
			for (short int i = 0; i < profile.appName.length(); i++) saveProfile << (char*)(&profile.appName[i]);
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

		//Write button mapping

		std::ofstream saveButtonMapping(std::format("Game Profiles/{}/mapping.txt", profile.profileName.c_str()));
		if (saveButtonMapping.is_open()) {
			for (int i = 0; i < ARRAYSIZE(profile.buttonMapping); i++)
				saveButtonMapping << profile.buttonMapping[i] << '\n';
			for(int i = 0;i < ARRAYSIZE(profile.rumbleButton);i++)
				saveButtonMapping << profile.rumbleButton[i] << '\n';

			saveButtonMapping.close();
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
			std::getline(writeStrings, profileName, '\n');
			memcpy(&currentProfile.profileName[0], &profileName[0], profileName.length());

			//It just works if I add this idk

			writeStrings.ignore();
			writeStrings.unget();
			//
			std::getline(writeStrings, appName, '\n');

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

	std::ifstream loadButtonMapping(std::format("{}/mapping.txt", dirEntry));
	if (loadButtonMapping.is_open()) {
		int buttonMappingTemp[20]{};
		int buttonRumble[10]{};
		for (int i = 0; i < ARRAYSIZE(currentProfile.buttonMapping); i++)
			loadButtonMapping >> buttonMappingTemp[i];
		for (int i = 0; i < ARRAYSIZE(currentProfile.buttonMapping); i++)
			currentProfile.buttonMapping[i] = buttonMappingTemp[i];

		for (int i = 0; i < ARRAYSIZE(currentProfile.rumbleButton); i++)
			loadButtonMapping >> buttonRumble[i];
		for (int i = 0; i < ARRAYSIZE(currentProfile.rumbleButton); i++)
			currentProfile.rumbleButton[i] = buttonRumble[i];
		
		loadButtonMapping.close();
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
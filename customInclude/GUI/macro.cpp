#include "macro.h"
#include "GUI/functionality.h"

void macroEditor(Macros& Macro) {


	
}

Macros macroMaker() {

	srand(time(NULL));
	Macros newMacro{};
	newMacro.Name = std::format("Test{}",int(rand()));

	return newMacro;
}
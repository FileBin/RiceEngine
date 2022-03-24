#include "pch.h"

#include <GameEngine/Core.h>
#include <Scripts/PreInitScript.h>

using namespace Game;

int main(int argc, char** argv[]) {

#ifdef _DEBUG
	ShowWindow(GetConsoleWindow(), 1);
#endif
	SetProcessDPIAware();
	Core::RunNew(new PreInitScript());
	return 0;
}
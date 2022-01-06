#include "pch.h"

#include <GameEngine/Core.h>
#include <Scripts/PreInitScript.h>

using namespace Game;

int main(int argc, char** argv[]) {

#ifndef _DEBUG
	ShowWindow(GetConsoleWindow(), 0);
#else
	ShowWindow(GetConsoleWindow(), 1);
#endif

	SetProcessDPIAware();
	Core::RunNew(new PreInitScript());
	ShowWindow(GetConsoleWindow(), 0);

	return 0;
}
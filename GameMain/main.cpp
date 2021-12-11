#include <GameEngine/GameMain.h>
#include <MyRender.h>
#include <Scripts\MsaaTestScript.h>


using namespace GameEngine;

int main(int argc, char** argv[]) {
#ifndef _DEBUG
	ShowWindow(GetConsoleWindow(), 0);
#else
	ShowWindow(GetConsoleWindow(), 1);
#endif

	SetProcessDPIAware();

	GameMain engine;

	Log::LoadLocalization(Log::Localization::LoadFromJSON(L"language/en-EN/log.json"));
	MyRender* render = new MyRender();
	engine.SetRender(render);

	MsaaTestScript script;
	engine.AddScript(script);

	engine.Initialize();
	engine.Run();
	engine.Close();

	return 0;
}
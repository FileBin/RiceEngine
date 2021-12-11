#include <GameEngine/GameMain.h>
#include <MyRender.h>

using namespace GameEngine;

int main() {
	SetProcessDPIAware();
	ShowWindow(GetConsoleWindow(), 0);

	GameMain engine;

	Log::LoadLocalization(Log::Localization::LoadFromJSON(L"language/en-EN/log.json"));
	MyRender* render = new MyRender();
	engine.SetRender(render);

	engine.Initialize();
	engine.Run();
	engine.Close();

	return 0;
}
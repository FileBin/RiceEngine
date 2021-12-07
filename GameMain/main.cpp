#include <GameFramework/GameMain.h>
#include <MyRender.h>

using namespace GameFramework;

int main() {
	ShowWindow(GetConsoleWindow(), 0);

	GameMain framework;

	MyRender* render = new MyRender();

	framework.SetRender(render);
	framework.Init();
	framework.Run();
	framework.Close();
	return 0;
}
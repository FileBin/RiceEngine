#undef RICE_SOURCE
#include "Rice/Engine.hpp"
#include "Rice/Engine/DefaultCoreLoader.hpp"

#include "Rice/Tests/Scenes/CubeTest.hpp"

int main() {
    using namespace Rice;
    using namespace Rice::Tests;
    ClientCore::runNew(new_ptr<DefaultCoreLoader>("CubeTest", CubeTestScene::create()));
	return 0;
}
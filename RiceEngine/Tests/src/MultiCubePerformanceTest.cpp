#include "Rice/Engine.hpp"
#include "Rice/Engine/DefaultCoreLoader.hpp"
#include "Rice/Tests/Scenes/MultiCubePerformanceTest.hpp"

int main() {
    using namespace Rice;
    using namespace Rice::Tests;
    ClientCore::runNew(new_ptr<DefaultCoreLoader>("CoreTest", MultiCubePerformanceTestScene::create()));
	return 0;
}
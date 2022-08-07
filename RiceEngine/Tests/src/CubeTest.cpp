#include "Rice/Engine/Core.hpp"
#include "Rice/Engine/DefaultCoreLoader.hpp"
#include "Rice/Tests/Scenes/CubeTest.hpp"

int main() {
    using namespace Rice;
    using namespace Rice::Tests;
    Core::runNew(new_ptr<DefaultCoreLoader>("CoreTest", CubeTestScene::create()));
	return 0;
}
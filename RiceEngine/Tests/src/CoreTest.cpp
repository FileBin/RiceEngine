#include "Rice/Engine/Core.hpp"
#include "Rice/Engine/DefaultCoreLoader.hpp"
#include "Rice/Tests/Scenes/QuadTest.hpp"

int main() {
    using namespace Rice;
    using namespace Rice::Tests;
    ClientCore::runNew(new_ptr<DefaultCoreLoader>("CoreTest", QuadTestScene::create()));
	return 0;
}
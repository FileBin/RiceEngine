/*
 * CoreTest.hpp
 *
 *  Created on: June 14, 2022
 *      Author: filebin
 */

#include "../stdafx.hpp"
#include "../Engine/Core.hpp"
#include "../Engine/DefaultCoreLoader.hpp"

#include "BetterCpp/Functions.hpp"
#include "Scenes/CubeTest.hpp"


NSP_TESTS_BEGIN

class CoreTest : public ICleanable {

public:
	static void runTest() {
        using namespace Rice;
		CoreTest program;

		program.entrypoint();
	}

	~CoreTest() {
		cleanup();
	}
private:

	void entrypoint() {
        Core::runNew(new_ref<DefaultCoreLoader>("CubeTest",new_ref<CubeTestScene>()));
	}

    void cleanup() override {}
};



NSP_TESTS_END


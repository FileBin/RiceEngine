/*
 * QuadTest.hpp
 *
 *  Created on: May 14, 2022
 *      Author: filebin
 */

#include "../stdafx.hpp"
#include "../Engine/Core.hpp"

#include "CoreTest/all.hpp"


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
        Core::runNew(new_ref<PreInitScript>());
	}

    void cleanup() override {}
};



NSP_TESTS_END


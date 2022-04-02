/*
 * HelloTriangle.hpp
 *
 *  Created on: 1 апр. 2022 г.
 *      Author: FileBinsLapTop
 */

#include "../stdafx.hpp"
#include "../Engine/Window.hpp"
#include "../GL/GraphicsManager.hpp"
#include "../Engine/Log.hpp"

NSP_TESTS_BEGIN

class HelloTriangle : public ICleanable {

public:
	static void runTest(){
		HelloTriangle program;
		program.entrypoint();
	}
private:
	~HelloTriangle() {
		cleanup();
	}

	void entrypoint() {
		Log::init();
		win.create({ "Vulkan" });
		Log::debug("Window created!");
		g_mgr.init(&win);
		while(win.update())
			loop();
	}

	void loop() {

	}

	void cleanup() override {
		g_mgr.cleanup();
		win.cleanup();
		Log::close();
	}

	Window win;
	Graphics::GraphicsManager g_mgr;
};



NSP_TESTS_END

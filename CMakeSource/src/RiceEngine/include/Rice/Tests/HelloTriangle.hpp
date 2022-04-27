/*
 * HelloTriangle.hpp
 *
 *  Created on: 1 Apr 2022
 *      Author: filebin
 */

#include "../stdafx.hpp"
#include "../Engine/Window.hpp"
#include "../GL/GraphicsManager.hpp"
#include "../GL/CommandBuffer.hpp"
#include "../GL/Shader.hpp"
#include "../Engine/Log.hpp"

NSP_TESTS_BEGIN

class HelloTriangle : public ICleanable {

public:
	static void runTest(){
		HelloTriangle program;
		try {
			program.entrypoint();
		} catch (::Rice::Exception& e) {
			Log::log(Log::Error, "Exception: {}\nFile: {}, Line: {} \nInfo: {}", e.GetMsg(), e.GetFile(), e.GetLine(), e.GetInfo());
		}
	}
private:
	~HelloTriangle() {
		cleanup();
	}

	void entrypoint() {
		using namespace Graphics;
		Log::init();
		win.create({ "Vulkan" });
		Log::debug("Window created!");
		g_mgr.init(&win);
		test_shader = new Shader(&g_mgr);

		test_shader->loadShader("shaders/triangle.vert.spv", Shader::Vertex);
		test_shader->loadShader("shaders/triangle.frag.spv", Shader::Fragment);
		test_shader->buildPipeline({ win.getWidth(), win.getHeight() });

		cmd = new CommandBuffer(&g_mgr);

		cmd->setActiveShader(test_shader);
		cmd->drawVertices(3);
		cmd->build();

		while(win.update())
			loop();
	}

	void loop() {
		if(!win.isResize()) {
			g_mgr.drawCmd(cmd);
			/*if(g_mgr.beginDraw()) {
				test_shader->setActive();
				g_mgr.draw(3);
				g_mgr.endDraw();
			}*/
		} else {
			Log::debug("Frame skipped! window resize!");
		}
	}

	void cleanup() override {
		g_mgr.cleanup();
		win.cleanup();
		Log::close();
	}

	Window win;
	Graphics::pShader test_shader;
	Graphics::GraphicsManager g_mgr;
	Graphics::pCommandBuffer cmd;
};



NSP_TESTS_END

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
#include "../GL/Mesh.hpp"
#include "../Math.hpp"
#include "../GL/VertexBuffer.hpp"

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

		VertexList vertices = {
			new Vertex,
			new Vertex,
			new Vertex,
		};

		vertexBuffer = new VertexBuffer(&g_mgr, vertices);

		cmd = new CommandBuffer(&g_mgr);

		cmd->setActiveShader(test_shader);
		cmd->bindVertexBuffer(vertexBuffer);
		cmd->drawVertices(3);
		cmd->build();

		while(win.update())
			loop();
	}

	void loop() {
		using namespace Graphics;
		Vertex vert[3] = {
				{{1, 1, 0}, {1, 0, 0}},
				{{-1,1, 0}, {0, 1, 0}},
				{{0,-1, 0}, {0, 0, 1}},
		};

		constexpr float ph[] =  {0, 2*Math::PI/3, 4*Math::PI/3};

		float t = clock() * .0001f;

		float threshold = sin(t * .1f)*.65f - .5f;

		Vector3f color = {
				Math::clamp01((cos(t + ph[0]) - threshold)/(1-threshold)),
				Math::clamp01((cos(t + ph[1]) - threshold)/(1-threshold)),
				Math::clamp01((cos(t + ph[2]) - threshold)/(1-threshold)),
		};

		vert[0].data.norm = color;
		vert[1].data.norm = {color.y, color.z, color.x};
		vert[2].data.norm = {color.z, color.x, color.y};

		vertexBuffer->updateVertice(0, vert[0]);
		vertexBuffer->updateVertice(1, vert[1]);
		vertexBuffer->updateVertice(2, vert[2]);

		if(!win.isResize()) {
			g_mgr.drawCmd(cmd);
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
	Graphics::pVertexBuffer vertexBuffer;
	Graphics::pShader test_shader;
	Graphics::GraphicsManager g_mgr;
	Graphics::pCommandBuffer cmd;
};



NSP_TESTS_END

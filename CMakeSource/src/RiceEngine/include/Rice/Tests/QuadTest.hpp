/*
 * QuadTest.hpp
 *
 *  Created on: May 14, 2022
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

class QuadTest : public ICleanable {

public:
	static void runTest(){
		QuadTest program;
		try {
			program.entrypoint();
		} catch (::Rice::Exception& e) {
			Log::log(Log::Error, "Exception: {}\nFile: {}, Line: {} \nInfo: {}", e.msg(), e.file(), e.line(), e.info());
		}
	}

	~QuadTest() {
		cleanup();
	}
private:

	void entrypoint() {
		using namespace Graphics;
		using namespace bettercpp;
		Log::init();
		pWindow win_ref = pWindow(&win);
		//if(in_stack)
			//win_ref.setDestoyer(PtrDestroyerType::DontDelete);

		win.create({ "Vulkan" });
		Log::debug("Window created!");
		pGraphicsManager g_mgr_ref = pGraphicsManager(&g_mgr);
		//if(in_stack)
			//g_mgr_ref.setDestoyer(PtrDestroyerType::DontDelete);
		g_mgr.init(win_ref);
		test_shader = new_ref<Shader>(g_mgr_ref);

		test_shader->loadShader("shaders/triangle.vert.spv", Shader::Vertex);
		test_shader->loadShader("shaders/triangle.frag.spv", Shader::Fragment);
		test_shader->buildPipeline(win.getSize());

		using pVert = RefPtr<IVertex>;

		VertexList vertices = {
				new_ref<Vertex>(Vector3f(-1,-1,0), Vector3f(0,1,0)),
				new_ref<Vertex>(Vector3f(-1,1,0), Vector3f(1,0,0)),
				new_ref<Vertex>(Vector3f(1,1,0), Vector3f(0,0,1)),
				new_ref<Vertex>(Vector3f(1,-1,0), Vector3f(0,1,0)),
		};

		List<index_t> indexes({
			0,1,2,
			2,0,3
		});

		vertexBuffer = new_ref<VertexBuffer>(g_mgr_ref, vertices);
		indexBuffer = new_ref<IndexBuffer>(g_mgr_ref, indexes);

		cmd = new_ref<CommandBuffer>(g_mgr_ref);

		cmd->setActiveShader(test_shader);
		cmd->bindVertexBuffer(vertexBuffer);
		cmd->bindIndexBuffer(indexBuffer);
		cmd->drawIndexed(6);
		cmd->build();

		while(win.update())
			loop();
	}

	void loop() {
		using namespace Graphics;
		if(!win.isResize()) {
			g_mgr.drawCmd(cmd);
		} else {
			Log::debug("Frame skipped! window resize!");
		}
	}

	void cleanup() override {
		cmd.release();
		vertexBuffer.release();
		test_shader.release();
		g_mgr.cleanup();
		win.cleanup();
		Log::close();
	}

	Window win;
	Graphics::pVertexBuffer vertexBuffer;
	Graphics::pIndexBuffer indexBuffer;
	Graphics::pShader test_shader;
	Graphics::GraphicsManager g_mgr;
	Graphics::pCommandBuffer cmd;
};



NSP_TESTS_END


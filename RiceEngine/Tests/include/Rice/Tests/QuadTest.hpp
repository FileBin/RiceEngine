/*
 * QuadTest.hpp
 *
 *  Created on: May 14, 2022
 *      Author: filebin
 */

#include "Rice/stdafx.hpp"

#include "Rice/Engine/Log.hpp"
#include "Rice/Engine/Window.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Mesh.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/GL/UniformBuffer.hpp"
#include "Rice/GL/VertexBuffer.hpp"

struct VertConstData {
    Matrix4x4f renderMatrix;
};

NSP_TESTS_BEGIN

class QuadTest : public ICleanable {

  public:
    static void runTest() {
        // test instance
        QuadTest program;
        try {
            // run test
            program.entrypoint();
        } catch (Util::Exception &e) {
            // catch exception
            Log::log(Log::Error, "Exception: {}\nFile: {}, Line: {} \nInfo: {}", e.msg(),
                     e.file(), e.line(), e.info());
        }
    }

    ~QuadTest() { cleanup(); }

  private:
    void entrypoint() {
        using namespace Graphics;

        // log initialization
        Log::init();

        // graphics manager initialization
        g_mgr = GraphicsManager::create();

        // constant frame data initialization
        const_data.renderMatrix =
            Matrix4x4::scale({.5, .5, .5}) * Matrix4x4::translation({0, 0, -.5});

        // window creation
        win = Window::create({"QuadTest"});

        // window initialization
        g_mgr->init(win);

        // shader creation
        test_shader = new_ptr<Shader>(g_mgr);

        // loading vertex shader
        test_shader->loadShader("shaders/quad_test.vert.spv", Shader::Vertex);
        // loading fragment shader
        test_shader->loadShader("shaders/quad_test.frag.spv", Shader::Fragment);
        // adding uniform buffer binding to shader
        test_shader->addUniformBuffer(0, Shader::Vertex);
        // setup shader layout
        test_shader->setVertexStrideAndLayout<Vertex>();
        // build shader
        test_shader->build();

        // uniform buffer creation
        uniformBuffer = new_ptr<UniformBuffer>(g_mgr, sizeof(VertConstData));
        // uniform buffer initialization with const buffer data
        uniformBuffer->updateData<VertConstData>(const_data);

        // cube vertices array
        VertexListT<Vertex> vertices({
            {Vector3f(-1, -1, 0), Vector3f(0, 1, 0)},
            {Vector3f(-1, 1, 0), Vector3f(1, 0, 0)},
            {Vector3f(1, 1, 0), Vector3f(0, 0, 1)},
            {Vector3f(1, -1, 0), Vector3f(0, 1, 1)},
        });

        // cube index
        vec<index_t> indexes({0, 1, 2, 3, 0, 2, 2, 1, 0, 2, 0, 3});

        vertexBuffer = new_ptr<VertexBuffer>(g_mgr, vertices);
        indexBuffer = new_ptr<IndexBuffer>(g_mgr, indexes);

        cmd = new_ptr<CommandBuffer>(g_mgr);
        cmd->beginRenderPass({0, 0, 1, 1});
        cmd->clearRenderTarget({0.1, 0.4, 0.8}, 1);

        cmd->setActiveShader(test_shader);
        cmd->bindVertexBuffer(vertexBuffer);
        cmd->bindIndexBuffer(indexBuffer);

        // cmd->pushConstants<VertConstData>(const_data, test_shader);
        cmd->bindUniformBuffer(uniformBuffer, 0);
        cmd->drawIndexed(indexBuffer);

        cmd->endRenderPass();
        cmd->buildAll();

        while (win->isOpen())
            loop();
    }

    void loop() {
        using namespace Graphics;
        using namespace std::chrono;

        g_mgr->update();

        static auto startTime = high_resolution_clock::now();

        auto currentTime = high_resolution_clock::now();
        double time = duration<double, seconds::period>(currentTime - startTime).count();

        VertConstData data = {const_data.renderMatrix};

        data.renderMatrix = Matrix4x4::scale({sin(time), cos(time), 1}) * data.renderMatrix;

        uniformBuffer->updateData<VertConstData>(data);

        g_mgr->executeCmd(cmd);
    }

    void cleanup() override { Log::close(); }

    ptr<Window> win;
    VertConstData const_data = {Matrix4x4f::identity};
    ptr<Graphics::VertexBuffer> vertexBuffer;
    ptr<Graphics::IndexBuffer> indexBuffer;
    ptr<Graphics::UniformBuffer> uniformBuffer;
    ptr<Graphics::Shader> test_shader;
    ptr<Graphics::GraphicsManager> g_mgr;
    ptr<Graphics::CommandBuffer> cmd;
};

NSP_TESTS_END

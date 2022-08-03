/*
 * QuadTest.hpp
 *
 *  Created on: May 14, 2022
 *      Author: filebin
 */

#include "Rice/Engine/Log.hpp"
#include "Rice/Engine/Window.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Mesh.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/GL/UniformBuffer.hpp"
#include "Rice/GL/VertexBuffer.hpp"
#include "Rice/Math/Math.hpp"
#include "Rice/defines.h"
#include "Rice/stdafx.hpp"

struct VertConstData {
    Matrix4x4f renderMatrix;
};

NSP_TESTS_BEGIN

class QuadTest : public ICleanable {

  public:
    static void runTest() {
        QuadTest program;
        try {
            program.entrypoint();
        } catch (Util::Exception &e) {
            Log::log(Log::Error, "Exception: {}\nFile: {}, Line: {} \nInfo: {}",
                     e.msg(), e.file(), e.line(), e.info());
        }
    }

    ~QuadTest() { cleanup(); }

  private:
    void entrypoint() {
        using namespace Graphics;
        Log::init();

        g_mgr = GraphicsManager::create();

        const_data.renderMatrix = Matrix4x4::scale({.5, .5, .5}) *
                                  Matrix4x4::translation({0, 0, -.5});

        win = Window::create({"QuadTest"});
        // if(in_stack)
        // g_mgr_ref.setDestoyer(PtrDestroyerType::DontDelete);
        g_mgr->init(win);
        test_shader = new_ptr<Shader>(g_mgr);

        test_shader->loadShader("shaders/quad_test.vert.spv", Shader::Vertex);
        test_shader->loadShader("shaders/quad_test.frag.spv", Shader::Fragment);
        test_shader->addUniformBuffer(0, Shader::Vertex);
        test_shader->setVertexStrideAndLayout<Vertex>();
        test_shader->build();

        uniformBuffer = new_ptr<UniformBuffer>(g_mgr, sizeof(VertConstData));
        uniformBuffer->setShader(test_shader);
        uniformBuffer->setBinding(0, sizeof(VertConstData));
        uniformBuffer->build();
        uniformBuffer->updateDataAll<VertConstData>(const_data);

        VertexListT<Vertex> vertices({
            {Vector3f(-1, -1, 0), Vector3f(0, 1, 0)},
            {Vector3f(-1, 1, 0), Vector3f(1, 0, 0)},
            {Vector3f(1, 1, 0), Vector3f(0, 0, 1)},
            {Vector3f(1, -1, 0), Vector3f(0, 1, 1)},
        });

        vec<index_t> indexes({0, 1, 2, 2, 0, 3});

        vertexBuffer = new_ptr<VertexBuffer>(g_mgr, vertices);
        indexBuffer = new_ptr<IndexBuffer>(g_mgr, indexes);

        cmd = new_ptr<CommandBuffer>(g_mgr);

        cmd->setActiveShader(test_shader);
        cmd->bindVertexBuffer(vertexBuffer);
        cmd->bindIndexBuffer(indexBuffer);

        // cmd->pushConstants<VertConstData>(const_data, test_shader);
        cmd->bindUniformBuffer(uniformBuffer);
        cmd->drawIndexed(indexBuffer);
        cmd->buildAll();

        while (win->isOpen())
            loop();
    }

    void loop() {
        using namespace Graphics;

        g_mgr->update();

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        double time =
            std::chrono::duration<double, std::chrono::seconds::period>(
                currentTime - startTime)
                .count();

        VertConstData data = {const_data.renderMatrix};

        data.renderMatrix =
            Matrix4x4::scale({sin(time), cos(time), 1}) * data.renderMatrix;

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

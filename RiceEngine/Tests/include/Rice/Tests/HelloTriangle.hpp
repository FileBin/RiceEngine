/*
 * HelloTriangle.hpp
 *
 *  Created on: 1 Apr 2022
 *      Author: filebin
 */

#include "Rice/stdafx.hpp"

#include "Rice/Engine/Log.hpp"
#include "Rice/Engine/Window.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Mesh.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/GL/VertexBuffer.hpp"
#include "Rice/Math/Math.hpp"

NSP_TESTS_BEGIN

class HelloTriangle : public ICleanable {

  public:
    static void runTest() {
        ptr<HelloTriangle> program{new HelloTriangle};
        try {
            program->entrypoint();
            program.release();
        } catch (::Rice::Exception &e) {
            Log::log(Log::Error, "Exception: {}\nFile: {}, Line: {} \nInfo: {}", e.msg(), e.file(),
                     e.line(), e.info());
        }
    }

    ~HelloTriangle() { cleanup(); }

  private:
    bool in_stack = false;
    HelloTriangle(bool is_in_stack = false) { in_stack = is_in_stack; }

    void entrypoint() {
        using namespace Rice::Graphics;
        Log::init();
        pWindow win_ref = pWindow(&win);
        // if(in_stack)
        // win_ref.setDestoyer(PtrDestroyerType::DontDelete);

        win.create({"Vulkan"});
        Log::debug("Window created!");
        pGraphicsManager g_mgr_ref = pGraphicsManager(&g_mgr);
        // if(in_stack)
        // g_mgr_ref.setDestoyer(PtrDestroyerType::DontDelete);
        g_mgr.init(win_ref);
        test_shader = new_ref<Shader>(g_mgr_ref);

        test_shader->loadShader("shaders/triangle.vert.spv", Shader::Vertex);
        test_shader->loadShader("shaders/triangle.frag.spv", Shader::Fragment);
        test_shader->setVertexStrideAndLayout<Vertex>();
        test_shader->build();

        VertexListT<Vertex> vertices({{{1, 1, 0}}, {{-1, 1, 0}}, {{0, -1, 0}}});

        vertexBuffer = new_ref<VertexBuffer>(g_mgr_ref, vertices);

        cmd = new_ref<CommandBuffer>(g_mgr_ref);

        cmd->setActiveShader(test_shader);
        cmd->bindVertexBuffer(vertexBuffer);
        cmd->drawVertices(3);
        cmd->buildAll();

        while (win.update())
            loop();
    }

    void loop() {
        /*using namespace Graphics;
        VertexListT<Vertex> vertices({
            {{1, 1, 0}},
            {{-1, 1, 0}},
            {{0, -1, 0}},
        });

        constexpr float ph[] = {0, 2 * Math::PI / 3, 4 * Math::PI / 3};

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        double time =
            std::chrono::duration<double, std::chrono::seconds::period>(currentTime - startTime)
                .count();
        float t = time * 4;

        float threshold = sin(t * .1f) * .65f - .5f;

        float r = Math::clamp01((cos(t + ph[0]) - threshold) / (1 - threshold));
        float g = Math::clamp01((cos(t + ph[1]) - threshold) / (1 - threshold));
        float b = Math::clamp01((cos(t + ph[2]) - threshold) / (1 - threshold));

        vertices.getVertex(0).norm = {r, g, b};
        vertices.getVertex(1).norm = {g, b, r};
        vertices.getVertex(2).norm = {b, r, g};

        vertexBuffer->updateVertices(vertices, 0);*/
        if (!win.isResize()) {
            g_mgr.executeCmd(cmd);
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
    Graphics::pShader test_shader;
    Graphics::GraphicsManager g_mgr;
    Graphics::pCommandBuffer cmd;
};

NSP_TESTS_END

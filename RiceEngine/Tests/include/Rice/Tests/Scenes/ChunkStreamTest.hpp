/*
 * ChunkStreamTest.hpp — scene for the chunk-streaming smoke test.
 * See Tests/src/ChunkStreamTest.cpp for what the run does.
 */

#include <SDL.h>

#include <cmath>
#include <cstdio>
#include <numeric>
#include <utility>

#undef RICE_SOURCE

#include "Rice/Engine.hpp"
#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Mesh.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/GL/RenderingMesh.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/Scene/ClientScene.hpp"
#include "Rice/Scene/Components/Camera.hpp"
#include "Rice/Scene/Components/ModelRender.hpp"
#include "Rice/Scene/Components/MonoScript.hpp"
#include "Rice/Scene/Components/Transform.hpp"
#include "Rice/Scene/Scene.hpp"

#pragma once

NSP_TESTS_BEGIN

/* Collected by the scene, read by main() for the exit code. */
struct ChunkStreamStats {
    inline static uint frames = 0;
    inline static uint created = 0;
    inline static uint destroyed = 0;
    inline static uint resetSubMeshCalls = 0;
    inline static uint partialUploads = 0;
    inline static uint failures = 0;

    static void fail(const char *what) {
        ++failures;
        Log::log(Log::Error, "ChunkStreamTest: {}", what);
    }
};

namespace chunk_stream {

/* A triangle-ish strip of `count` vertices/indices. Every count is a multiple
 * of 3 (6, 12, 18, 24) so a triangle list stays well formed. */
inline vec<Graphics::Vertex> ringVertices(uint count, dbl phase) {
    vec<Graphics::Vertex> verts(count);
    const dbl step = 6.283185307179586 / std::max(3u, count);
    for (uint i = 0; i < count; ++i) {
        const dbl a = step * i + phase;
        Graphics::Vertex v{};
        v.pos = Vector3f{(float)(std::cos(a) * .8), (float)(std::sin(a) * .8),
                         (float)(.2 * (i % 3))};
        v.norm = Vector3f{0, 0, 1};
        v.tex_coord0 = Vector2f{(float)i / count, 0};
        verts[i] = v;
    }
    return verts;
}

inline ptr<Graphics::Mesh> ringMesh(uint count, dbl phase) {
    vec<Graphics::Vertex> verts = ringVertices(count, phase);
    vec<index_t> indices(count);
    std::iota(indices.begin(), indices.end(), 0);
    return new_ptr<Graphics::Mesh>(Graphics::VertexListT<Graphics::Vertex>(verts), indices);
}

/* Shared cube used by every streamed chunk object. */
inline ptr<Graphics::Mesh> sharedCube() {
    static ptr<Graphics::Mesh> cube = Graphics::Mesh::cube().clone();
    return cube;
}

} // namespace chunk_stream

class ChunkStreamScript : public virtual Components::MonoScript {
  public:
    static constexpr uint kFrames = 120;   // frames the demo runs
    static constexpr uint kPerFrame = 32;  // objects created *and* destroyed per frame
    static constexpr uint kLifetime = 4;   // frames a chunk object stays alive

    ptr<Graphics::Material> material;
    ptr<Components::ModelRender> streamer; // geometry replaced every frame
    ptr<Components::ModelRender> partial;  // constant size, in-place uploads

    void onUpdate() override {
        auto scene = getClientScene();
        const uint frame = ChunkStreamStats::frames;

        checkRemovals(scene, frame);

        // 1) 32 new objects from inside onUpdate. This used to self-deadlock:
        //    root->createEmpty() took a unique_lock on the root's shared_mutex,
        //    which this update pass holds shared, and addComponent() pushed into
        //    the component vector being iterated.
        for (uint i = 0; i < kPerFrame; ++i) {
            auto obj = scene->createEnabled(fmt::format("chunk {}.{}", frame, i));
            if (!obj) {
                ChunkStreamStats::fail("createEnabled returned null");
                continue;
            }
            if (scene->getRegistered(obj->getUUID()) != obj)
                ChunkStreamStats::fail("a created object is not registered under its uuid");

            auto t = new_ptr<Components::Transform>();
            t->setPosition({(dbl)(i % 8) - 3.5, (dbl)((i / 8) % 4) - 1.5, (dbl)(frame % 4) - 1.5});
            auto mr = new_ptr<Components::ModelRender>();

            obj->addComponent(t);
            obj->addComponent(mr);

            auto model = new_ptr<Graphics::Model>();
            model->setSubMeshesCount(1);
            model->setSubMesh(chunk_stream::sharedCube(), 0);
            mr->setModel(model);
            mr->setMaterial(material, 0);

            live.push_back(obj);
            born.push_back(frame);
            ++ChunkStreamStats::created;
        }

        // 2) destroy the objects that outlived kLifetime — also from onUpdate.
        //    destroy() only marks; the next preUpdate tears the object down and
        //    unregisters its mesh.
        for (size_t k = 0; k < live.size();) {
            if (born[k] + kLifetime > frame) {
                ++k;
                continue;
            }
            auto obj = live[k];
            if (scene->getRegistered(obj->getUUID()) != obj)
                ChunkStreamStats::fail("a live object lost its registration");
            pendingRemoval.emplace_back(frame, obj->getUUID());
            obj->destroy();
            live.erase(live.begin() + k);
            born.erase(born.begin() + k);
            ++ChunkStreamStats::destroyed;
        }
        if (frame >= kLifetime && live.size() != kPerFrame * kLifetime)
            ChunkStreamStats::fail("unexpected number of live streamed objects");

        // 3) replace the streamer's geometry with a different index count every
        //    frame: the GPU buffers are rebuilt and the recorded draw command
        //    has to be re-recorded (index count is baked at record time).
        static constexpr uint kCounts[] = {6, 12, 18, 24};
        const uint indexCount = kCounts[frame % 4];
        auto mesh = chunk_stream::ringMesh(indexCount, (dbl)(frame % 11) * .37);
        streamer->resetSubMesh(mesh, 0);

        if (streamer->getModel()->getSubMesh(0) != mesh)
            ChunkStreamStats::fail("resetSubMesh did not update the submesh");
        if (streamer->getMaterial(0) != material)
            ChunkStreamStats::fail("resetSubMesh dropped the material");

        auto rm = streamer->getRenderingMesh(0);
        if (!rm) {
            ChunkStreamStats::fail("the streamed submesh has no RenderingMesh");
        } else {
            if (!rm->getIndexBuffer() || !rm->getVertexBuffer())
                ChunkStreamStats::fail("buffer accessors returned null");
            else if (rm->getIndexBuffer()->indexCount() != indexCount)
                ChunkStreamStats::fail("the index buffer was not rebuilt with the new count");
            if (!rm->isCmdDirty())
                ChunkStreamStats::fail("resetSubMesh did not request a command re-record");
        }
        ++ChunkStreamStats::resetSubMeshCalls;

        // 4) Path A: constant-size partial upload through the buffer accessors.
        //    Same vertex/index count, so the recorded draw stays valid.
        auto prm = partial->getRenderingMesh(0);
        if (!prm) {
            ChunkStreamStats::fail("the partial-upload submesh has no RenderingMesh");
        } else {
            constexpr uint n = 24;
            auto verts = chunk_stream::ringVertices(n, (dbl)(frame % 5) * .5);
            Graphics::VertexListT<Graphics::Vertex> list{verts};
            prm->getVertexBuffer()->updateVertices(list, 0); // byte offset 0
            vec<index_t> indices(n);
            std::iota(indices.begin(), indices.end(), 0);
            prm->getIndexBuffer()->updateIndices(indices, 0); // element index 0
            if (prm->getIndexBuffer()->indexCount() != n)
                ChunkStreamStats::fail("a constant-size upload changed the index count");
            ++ChunkStreamStats::partialUploads;
        }

        ++ChunkStreamStats::frames;
        if (ChunkStreamStats::frames >= kFrames)
            finish();
    }

  private:
    vec<ptr<Object>> live;
    vec<uint> born;
    vec<std::pair<uint, UUID>> pendingRemoval;

    /* A destroyed object must disappear from the scene registry within a frame
     * or two (its teardown runs in the next preUpdate). Anything still
     * registered after that is a lost/dangling object. */
    void checkRemovals(ptr<ClientScene> scene, uint frame) {
        for (size_t k = 0; k < pendingRemoval.size();) {
            if (pendingRemoval[k].first + 2 > frame) {
                ++k;
                continue;
            }
            bool stillRegistered = true;
            try {
                scene->getRegistered(pendingRemoval[k].second);
            } catch (Util::Exception &) {
                stillRegistered = false;
            }
            if (stillRegistered)
                ChunkStreamStats::fail("a destroyed object is still registered two frames later");
            pendingRemoval.erase(pendingRemoval.begin() + k);
        }
    }

    void finish() {
        if (ChunkStreamStats::failures == 0)
            std::printf("ChunkStreamTest: %u frames streamed, %u objects created/destroyed, "
                        "%u geometry replacements\n",
                        ChunkStreamStats::frames, ChunkStreamStats::created,
                        ChunkStreamStats::resetSubMeshCalls);
        else
            std::printf("ChunkStreamTest: finished with %u failure(s)\n",
                        ChunkStreamStats::failures);

        // Close the window the same way a window manager would, so the engine
        // takes its normal shutdown path and ClientCore::run() returns.
        auto window = getClientEngine()->getInputManager()->getWindow();
        SDL_Event event{};
        event.type = SDL_WINDOWEVENT;
        event.window.event = SDL_WINDOWEVENT_CLOSE;
        event.window.windowID = SDL_GetWindowID(window->getHandle().get());
        SDL_PushEvent(&event);
    }
};

class ChunkStreamScene : public virtual Scene {
    ChunkStreamScene() = default;

  public:
    static ptr<ChunkStreamScene> create() { return ptr<ChunkStreamScene>(new ChunkStreamScene()); }

    void init() override {
        auto ren = getSceneRender();

        // --- camera: the voxel game's field of view ------------------------
        auto cam_obj = createEnabled("Camera");
        auto cam_transform = new_ptr<Components::Transform>();
        cam_transform->setPosition({0, 0, -9});
        auto cam = new_ptr<Components::Camera>();
        cam->setFov(90);
        cam->setNearPlane(0.01);
        cam->setFarPlane(1000);
        cam_obj->addComponent(cam_transform);
        cam_obj->addComponent(cam);
        if (cam->getFov() != 90 || cam->getNearPlane() != 0.01 || cam->getFarPlane() != 1000)
            ChunkStreamStats::fail("camera projection parameters were not applied");
        std::printf("ChunkStreamTest: camera fov=%g near=%g far=%g\n", cam->getFov(),
                    cam->getNearPlane(), cam->getFarPlane());
        setActiveCamera(cam);

        // --- one shader + one material, shared by every chunk -------------
        auto material = ren->getOrCreateMaterial(
            "chunk_stream", [](ptr<SceneRender> ren) -> ptr<Graphics::Material> {
                auto shader = ren->getOrCreateShader(
                    "chunk_stream", [](ptr<Graphics::Shader> shader) -> void {
                        shader->loadShader("shaders/default.vert.spv",
                                           Graphics::Shader::Vertex);
                        shader->loadShader("shaders/diffuse.frag.spv",
                                           Graphics::Shader::Fragment);
                        shader->addUniformBuffer(0, Graphics::Shader::Vertex);
                        shader->addUniformBuffer(1, Graphics::Shader::Fragment);
                        shader->setVertexStrideAndLayout<Graphics::Vertex>();
                        shader->build();
                    });
                auto m = new_ptr<Graphics::Material>(ren->getGraphicsManager(), shader);
                Vector4f egst = {0, 32, 1, 0};
                Vector4f color = {0.35f, 0.75f, 0.4f, 1.f};
                m->initUniformBuffer(std::make_tuple(color, egst));
                return m;
            });

        // --- the object whose geometry is replaced every frame -----------
        auto streamer = createEnabled("Streamer");
        auto streamer_t = new_ptr<Components::Transform>();
        streamer_t->setPosition({-2.5, 0, 0});
        auto streamer_mr = new_ptr<Components::ModelRender>();
        streamer->addComponent(streamer_t);
        streamer->addComponent(streamer_mr);
        auto streamer_model = new_ptr<Graphics::Model>();
        streamer_model->setSubMeshesCount(1);
        streamer_model->setSubMesh(chunk_stream::ringMesh(6, 0), 0);
        streamer_mr->setModel(streamer_model);
        streamer_mr->setMaterial(material, 0);

        // --- the constant-size object used for partial uploads -----------
        auto partial = createEnabled("Partial");
        auto partial_t = new_ptr<Components::Transform>();
        partial_t->setPosition({2.5, 0, 0});
        auto partial_mr = new_ptr<Components::ModelRender>();
        partial->addComponent(partial_t);
        partial->addComponent(partial_mr);
        auto partial_model = new_ptr<Graphics::Model>();
        partial_model->setSubMeshesCount(1);
        partial_model->setSubMesh(chunk_stream::ringMesh(24, 0), 0);
        partial_mr->setModel(partial_model);
        partial_mr->setMaterial(material, 0);

        // --- the script that streams the world --------------------------
        auto script = new_ptr<ChunkStreamScript>();
        script->material = material;
        script->streamer = streamer_mr;
        script->partial = partial_mr;
        auto driver = createEnabled("ChunkStreamer");
        driver->addComponent(script);
    }
};

NSP_TESTS_END

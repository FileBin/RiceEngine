#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/Util/defines.hpp"
#include "pch.h"

#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/Scene/ClientScene.hpp"
#include <Rice/GL/RenderingMesh.hpp>
#include <Rice/Scene/SceneRender.hpp>

NSP_ENGINE_BEGIN

SceneRender::SceneRender(ptr<ClientScene> scene, ptr<ClientEngine> engine)
    : scene(scene), engine(engine) {
    main_cmd = new_ptr<Graphics::CommandBuffer>(engine->getGraphicsManager());
    clear_cmd = new_ptr<Graphics::CommandBuffer>(engine->getGraphicsManager(), true);
    clear_cmd->clearRenderTarget({0.1, 0.3, 0.7}, 1);
    clear_cmd->buildAll();
    updateCmd();
}

SceneRender::ShaderQueue::ShaderQueue(ptr<Graphics::Shader> shader,
                                      ptr<Graphics::GraphicsManager> mgr)
    : shader(shader) {
    cmd = new_ptr<Graphics::CommandBuffer>(mgr, true);
}

SceneRender::MaterialQueue::MaterialQueue(ptr<Graphics::Material> mat) : material(mat) {}

void SceneRender::MaterialQueue::addCommands(ptr<Graphics::CommandBuffer> cmd) {
    auto ub = material->getUniformBuffer();
    if (ub) {
        cmd->bindUniformBuffer(ub, 1);
    }

    auto mc = mesh_collection.getCollectionWithGaps();
    for (auto ptr : mc) {
        if (ptr) {
            ptr->addCommands(cmd);
        }
    }
}

void SceneRender::ShaderQueue::updateCmd() {
    cmd->clear();
    cmd->setActiveShader(shader);
    for (auto &mq : materials) {
        mq.second.addCommands(cmd);
    }
    cmd->buildAll();
}

void SceneRender::registerMesh(ptr<Graphics::RenderingMesh> mesh) {
    auto mat = mesh->getMaterial();
    auto &sq = mainQueue.at(mat->getShader());
    auto &mq = sq.materials.at(mat);

    mesh->Register(mq.mesh_collection.registerPtr(mesh));
    need_rebuild = true;
}

void SceneRender::unregisterMesh(ptr<Graphics::RenderingMesh> mesh) {
    auto mat = mesh->getMaterial();
    auto &sq = mainQueue.at(mat->getShader());
    auto &mq = sq.materials.at(mat);
    mq.mesh_collection.unregister(mesh->Unregister());
    need_rebuild = true;
}

void SceneRender::updateCmd() {
    main_cmd->clear();
    main_cmd->beginRenderPass({0, 0, 1, 1}, true);
    main_cmd->executeCommandBuffer(clear_cmd);
    for (auto &sq : mainQueue) {
        sq.second.updateCmd();
        main_cmd->executeCommandBuffer(sq.second.cmd);
    }

    main_cmd->endRenderPass();

    need_rebuild = true;
}

void SceneRender::collectGeometryChanges() {
    for (auto &sq : mainQueue)
        for (auto &mq : sq.second.materials)
            for (auto &mesh : mq.second.mesh_collection.getCollectionWithGaps())
                if (mesh && mesh->consumeCmdDirty())
                    need_rebuild = true;
}

void SceneRender::rebuildCmd() {
    if (need_rebuild) {
        updateCmd();
        main_cmd->buildAll();
        need_rebuild = false;
    }
}

void SceneRender::draw(ptr<Components::Camera> camera) {

    auto g_mgr = getEngine()->getGraphicsManager();
    g_mgr->sync();
    // Command buffers are re-recorded only when the queue actually changed
    // (meshes/materials registered, or a mesh got new geometry of a different
    // size); per-frame rebuilds re-recorded every draw command and re-allocated
    // its descriptor set.
    collectGeometryChanges();
    rebuildCmd();
    uint count = update(camera);
    g_mgr->executeCmd(main_cmd);
    // TODO add transparent queue
}

uint SceneRender::update(ptr<Components::Camera> camera) {
    uint count = 0;
    if (!camera)
        return count;

    // Per-frame, per-scene constants. Both used to be recomputed inside the
    // per-mesh loop: getProjectionMatrix() queries SDL_GetWindowSize + tan(),
    // getViewMatrix() inverts the 4x4 transformation matrix - so a scene with N
    // meshes paid for 2N window queries and N matrix inversions per frame.
    // Computing them once here yields the same values (the window cannot resize
    // in the middle of a frame) and keeps resize behaviour unchanged: the next
    // frame queries the new size again.
    const Matrix4x4 view = camera->getViewMatrix();
    const Matrix4x4 projection = camera->getProjectionMatrix();

    for (auto &sq : mainQueue)
        for (auto &mq : sq.second.materials) {
            auto &coll = mq.second.mesh_collection.getCollectionWithGaps();
            for (auto &ptr : coll) {
                if (ptr) {
                    ptr->updateConstBuffer(view, projection);
                    ptr->updateCmdBuffer();
                    count++;
                }
            }
        }
    return count;
}

void SceneRender::cleanup() { mainQueue.clear(); }

ptr<Graphics::Shader> SceneRender::getShader(String name) {
    try {
        return shaders.at(name);
    } catch (std::out_of_range) {
        THROW_EXCEPTION(fmt::format("Shader {} not found!", name).c_str());
    }
}

ptr<Graphics::Material> SceneRender::getMaterial(String name) {
    try {
        return materials.at(name);
    } catch (std::out_of_range) {
        THROW_EXCEPTION(fmt::format("Material {} not found!", name).c_str());
    }
}

ptr<Graphics::Shader>
SceneRender::getOrCreateShader(String name,
                               std::function<void(ptr<Graphics::Shader>)> shader_creator) {
    auto &sh = shaders[name];
    if (!sh) {
        sh.reset(new Graphics::Shader(getGraphicsManager())); // create shader
        shader_creator(sh); // call creator function to setup shader

        mainQueue.insert({sh, ShaderQueue(sh, getEngine()->getGraphicsManager())});
        need_rebuild = true;
    }
    return sh;
}

ptr<Graphics::Material> SceneRender::getOrCreateMaterial(
    String name, std::function<ptr<Graphics::Material>(ptr<SceneRender>)> material_factory) {
    auto &m = materials[name];
    if (!m) {
        m = material_factory(shared_from_this());
        auto &shader_q = mainQueue.at(m->getShader());
        shader_q.materials.insert({m, MaterialQueue(m)});
        need_rebuild = true;
    }
    return m;
}

ptr<ClientEngine> SceneRender::getEngine() {
    auto engine_lock = engine.lock();
    if (!engine_lock) {
        THROW_NULL_PTR_EXCEPTION(engine_lock.get());
    }
    return engine_lock;
}

ptr<Graphics::GraphicsManager> SceneRender::getGraphicsManager() {
    return getEngine()->getGraphicsManager();
}

NSP_ENGINE_END
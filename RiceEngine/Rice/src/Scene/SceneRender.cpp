#include "Rice/GL/GraphicsManager.hpp"
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
                                      ptr<Graphics::GraphicsManager> mgr) {
    setShader = new_ptr<Graphics::CommandBuffer>(mgr, true);
    setShader->setActiveShader(shader);
    setShader->buildAll();
}

SceneRender::MaterialQueue::MaterialQueue(ptr<Graphics::Material> mat,
                                          ptr<Graphics::GraphicsManager> mgr) {
    auto ub = mat->getUniformBuffer();
    if (ub) {
        setMaterial = new_ptr<Graphics::CommandBuffer>(mgr, true);
        setMaterial->bindUniformBuffer(ub, 1);
        setMaterial->buildAll();
    }
}

void SceneRender::registerMesh(ptr<Graphics::RenderingMesh> mesh) {
    auto mat = mesh->getMaterial();
    auto sq = mainQueue.at(mat->getShader());
    auto mq = sq.materials.at(mat);

    mesh->Register(mq.mesh_collection.registerPtr(mesh));
    updateCmd();
}

void SceneRender::unregisterMesh(ptr<Graphics::RenderingMesh> mesh) {
    auto mat = mesh->getMaterial();
    auto sq = mainQueue.at(mat->getShader());
    auto mq = sq.materials.at(mat);
    mq.mesh_collection.unregister(mesh->Unregister());
    updateCmd();
}

void SceneRender::updateCmd() {
    main_cmd->clear();
    main_cmd->beginRenderPass({0, 0, 1, 1}, true);
    main_cmd->executeCommandBuffer(clear_cmd);
    for (auto &sq : mainQueue) {
        main_cmd->executeCommandBuffer(sq.second.setShader);
        for (auto &mq : sq.second.materials) {
            if (mq.second.setMaterial)
                main_cmd->executeCommandBuffer(mq.second.setMaterial);
            auto mesh_collection = mq.second.mesh_collection.getCollectionWithGaps();
            for (auto ptr : mesh_collection) {
                if (ptr) {
                    main_cmd->executeCommandBuffer(ptr->getCmd());
                }
            }
        }
    }

    main_cmd->endRenderPass();

    need_rebuild = true;
}

void SceneRender::rebuildCmd() {
    if (need_rebuild) {
        main_cmd->buildAll();
        need_rebuild = false;
    }
}

void SceneRender::draw(ptr<Components::Camera> camera) {
    auto g_mgr = getEngine()->getGraphicsManager();
    g_mgr->sync();
    need_rebuild = true;
    rebuildCmd();
    uint count = update(camera);
    g_mgr->executeCmd(main_cmd);
    // TODO add transparent queue
}

uint SceneRender::update(ptr<Components::Camera> camera) {
    uint count = 0;
    for (auto &sq : mainQueue)
        for (auto &mq : sq.second.materials) {
            auto coll = mq.second.mesh_collection.getCollectionWithGaps();
            for (auto ptr : coll) {
                if (ptr) {
                    auto proj = camera->getProjectionMatrix();
                    ptr->updateConstBuffer(camera->getViewMatrix(), proj);
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
        THROW_EXCEPTION(fmt::format("Shader {} not found!", name.toUTF8String()).c_str());
    }
}

ptr<Graphics::Material> SceneRender::getMaterial(String name) {
    try {
        return materials.at(name);
    } catch (std::out_of_range) {
        THROW_EXCEPTION(fmt::format("Material {} not found!", name.toUTF8String()).c_str());
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
    }
    return sh;
}

ptr<Graphics::Material> SceneRender::getOrCreateMaterial(
    String name, std::function<ptr<Graphics::Material>(ptr<SceneRender>)> material_factory) {
    auto &m = materials[name];
    if (!m) {
        m = material_factory(shared_from_this());
        auto shader_q = mainQueue.at(m->getShader());
        shader_q.materials.insert({m, MaterialQueue(m, getEngine()->getGraphicsManager())});
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
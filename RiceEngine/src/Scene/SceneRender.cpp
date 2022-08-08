#include "pch.h"

#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/Engine/InputManager.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/Math/Vectors/Vector2.hpp"
#include "Rice/Scene/ClientScene.hpp"
#include "Rice/Scene/Components/Camera.hpp"
#include <Rice/GL/RenderingMesh.hpp>
#include <Rice/Scene/SceneRender.hpp>

NSP_ENGINE_BEGIN

SceneRender::SceneRender(ptr<ClientScene> scene, ptr<ClientEngine> engine)
    : scene(scene), engine(engine) {
    begin_cmd =
        new_ptr<Graphics::CommandBuffer>(engine->getGraphicsManager(), true);
    begin_cmd->buildAll();
}

void SceneRender::registerMesh(ptr<Graphics::RenderingMesh> mesh) {
    mesh->Register(mesh_collection.registerPtr(mesh));
}

void SceneRender::unregisterMesh(ptr<Graphics::RenderingMesh> mesh) {
    mesh_collection.unregister(mesh->Unregister());
}

void SceneRender::draw(ptr<Components::Camera> camera) {
    auto g_mgr = getEngine()->getGraphicsManager();
    g_mgr->sync();
    uint count = update(camera);
    g_mgr->executeCmds(getCmds(count));
    // TODO add transparent queue
}

uint SceneRender::update(ptr<Components::Camera> camera) {
    uint count = 0;
    auto coll = mesh_collection.getCollectionWithGaps();
    for (auto ptr : coll) {
        if (ptr) {
            auto proj = camera->getProjectionMatrix();
            ptr->updateConstBuffer(camera->getViewMatrix(), proj);
            ptr->updateCmdBuffer();
            count++;
        }
    }
    return count;
}

vec<ptr<Graphics::CommandBuffer>> SceneRender::getCmds(uint count) {
    vec<ptr<Graphics::CommandBuffer>> output(count + 1);
    uint i = 0;
    output[i++] = begin_cmd;
    auto coll = mesh_collection.getCollectionWithGaps();
    for (auto ptr : coll) {
        if (ptr) {
            output[i++] = ptr->getCmd();
        }
    }
    return output;
}

void SceneRender::cleanup() { mesh_collection.cleanup(); }

ptr<Graphics::Shader> SceneRender::getOrCreateShader(
    String name, std::function<void(ptr<Graphics::Shader>)> shader_creator) {
    auto it = shaders.find(name); // try to find shader in map
    if (it == shaders.end()) {
        ptr<Graphics::Shader> shader{
            new Graphics::Shader(getGraphicsManager())}; // create shader
        shader_creator(shader); // call creator function to setup shader
        shaders[name] = shader; // add to map
        return shader;
    }
    return it->second;
}

ptr<Graphics::Material> SceneRender::getOrCreateMaterial(
    String name,
    std::function<ptr<Graphics::Material>(ptr<SceneRender>)> material_factory) {
    auto it = materials.find(name); // try to find material in map
    if (it == materials.end()) {
        return materials[name] =
                   material_factory(shared_from_this()); // add to map
    }
    return it->second;
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
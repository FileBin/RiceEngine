#include "Rice/Engine/InputManager.hpp"
#include "Rice/Math/Vectors/Vector2.hpp"
#include "Rice/Scene/Components/Camera.hpp"
#include <Rice/GL/RenderingMesh.hpp>
#include <Rice/Scene/SceneRender.hpp>

NSP_ENGINE_BEGIN

SceneRender::SceneRender(ptr<Scene> scene, ptr<Engine> engine)
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
    auto g_mgr = engine->getGraphicsManager();
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

NSP_ENGINE_END
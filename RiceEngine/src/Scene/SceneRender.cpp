#include "Rice/Scene/Components/Camera.hpp"
#include <Rice/Scene/SceneRender.hpp>
#include <Rice/GL/RenderingMesh.hpp>

NSP_ENGINE_BEGIN

SceneRender::SceneRender(ptr<Scene> scene, ptr<Engine> engine)
    : scene(scene), engine(engine) {}

void SceneRender::registerMesh(ptr<Graphics::RenderingMesh> mesh) {
    mesh->Register(mesh_collection.registerPtr(mesh));
}

void SceneRender::unregisterMesh(ptr<Graphics::RenderingMesh> mesh) {
    mesh_collection.unregister(mesh->Unregister());
}

void SceneRender::draw(ptr<Components::Camera> camera) {
    uint count = update(camera);
    auto g_mgr = engine->getGraphicsManager();
    g_mgr->executeCmds(getCmds(count));
    // TODO add transparent queue
}

uint SceneRender::update(ptr<Components::Camera> camera) {
    uint count = 0;
    auto coll = mesh_collection.getCollectionWithGaps();
    for (auto ptr : coll) {
        if (ptr) {
            ptr->updateConstBuffer(camera->getViewMatrix(),
                                   camera->getProjectionMatrix());
            ptr->updateCmdBuffer();
            count++;
        }
    }
    return count;
}

vec<ptr<Graphics::CommandBuffer>> SceneRender::getCmds(uint count) {
    vec<ptr<Graphics::CommandBuffer>> output(count);
    uint i = 0;
    auto coll = mesh_collection.getCollectionWithGaps();
    for (auto ptr : coll) {
        if (ptr) {
            output[i++] = ptr->getCmd();
        }
    }
    return output;
}

NSP_ENGINE_END
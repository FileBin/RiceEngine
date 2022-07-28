#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/RenderingMesh.hpp"
#include "Rice/namespaces.h"
#include "pch.h"

#include "Rice/Scene/Components/Camera.hpp"
#include <Rice/Scene/SceneRender.hpp>

NSP_ENGINE_BEGIN

SceneRender::SceneRender(pScene scene, pEngine engine)
    : scene(scene), engine(engine) {}

void SceneRender::registerMesh(Graphics::pRenderingMesh mesh) {
    mesh->Register(mesh_collection.registerPtr(mesh));
}

void SceneRender::unregisterMesh(Graphics::pRenderingMesh mesh) {
    mesh_collection.unregister(mesh->Unregister());
}

void SceneRender::draw(Components::pCamera camera) {
    uint count = update(camera);
    auto g_mgr = engine->getGraphicsManager();
    g_mgr->executeCmds(getCmds(count));
    //TODO add transparent queue
}

uint SceneRender::update(Components::pCamera camera) {
    uint count = 0;
    auto coll = mesh_collection.getCollectionWithGaps();
    for (auto ptr : coll) {
        if (ptr.isNotNull()) {
            ptr->updateConstBuffer(camera->getViewMatrix(), camera->getProjectionMatrix());
            ptr->updateCmdBuffer();
            count++;
        }
    }
    return count;
}

vec<Graphics::pCommandBuffer> SceneRender::getCmds(uint count) {
    vec<Graphics::pCommandBuffer> output(count);
    uint i = 0;
    auto coll = mesh_collection.getCollectionWithGaps();
    for (auto ptr : coll) {
        if (ptr.isNotNull()) {
            output[i++] = ptr->getCmd();
        }
    }
    return output;
}

NSP_ENGINE_END
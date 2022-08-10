#include "pch.h"

using namespace Rice::Graphics;

NSP_COMPONENTS_BEGIN

ModelRender::ModelRender(ptr<Model> model) { this->model = model; }

ModelRender::ModelRender() {}

void ModelRender::onEnable() {
    auto g_mgr = getClientEngine()->getGraphicsManager();

    auto transform = getObject()->getComponent<Transform>();
    if (!transform) {
        THROW_EXCEPTION("Object has no Transform component!");
    }
    uint n = model->getSubMeshesCount();
    for (uint i = 0; i < n; i++) {
        auto mat = materials[i];
        auto mesh = model->getSubMesh(i);
        if (mat && !mesh->isEmpty()) {
            auto rm = new_ptr<Graphics::RenderingMesh>(
                g_mgr, mesh, mat, transform);
            rendering_meshes.push_back(rm);
            getSceneRender()->registerMesh(rm);
        }
    }
}

void ModelRender::onDisable() {
    for (auto &rm : rendering_meshes) {
        getSceneRender()->unregisterMesh(rm);
    }
    rendering_meshes.clear();
}

void ModelRender::setModel(ptr<Model> model) {
    this->model = model;
    // TODO set flag to update and mutex to protect from multiple threads
    if (isEnabled())
        return;
    updateRenderData();
}

void ModelRender::updateRenderData() {
    materials.resize(model->getSubMeshesCount());
}

ptr<Model> ModelRender::getModel() { return model; }

void ModelRender::setMaterial(ptr<Material> material, uint subMeshIdx) {
    materials[subMeshIdx] = material;
}

NSP_COMPONENTS_END
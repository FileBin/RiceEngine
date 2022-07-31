#include <Rice/Scene/Components/ModelRender.hpp>
#include <Rice/Engine/Engine.hpp>
using namespace Rice::Graphics;

NSP_COMPONENTS_BEGIN

ModelRender::ModelRender(ptr<Model> model) { this->model = model; }

ModelRender::ModelRender() {}

void ModelRender::onInit() {
    auto g_mgr = getEngine()->getGraphicsManager();

    uint n = model->getSubMeshesCount();
    for (uint i = 0; i < n; i++) {
        meshes.push_back(
            Graphics::RenderingMesh(g_mgr, model->getSubMesh(i), materials[i]));
    }
}

void ModelRender::setModel(ptr<Model> model) { this->model = model; }

ptr<Model> ModelRender::getModel() { return model; }

void ModelRender::setMaterial(ptr<Material> material, uint subMeshIdx) {
    materials[subMeshIdx] = material;
}

NSP_COMPONENTS_END
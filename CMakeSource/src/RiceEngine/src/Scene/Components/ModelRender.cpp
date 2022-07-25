#include "Rice/Scene/Components/Transform.hpp"
#include "Rice/namespaces.h"
#include "pch.h"

#include <Rice/Scene/Components/ModelRender.hpp>

NSP_COMPONENTS_BEGIN

typedef PackableComponent<ModelRender, RefTuple<Graphics::pModel, vec<Graphics::pMaterial>, vec<Graphics::RenderingMesh>>, RefTuple<pTransform>> base;

#define BASE_CTOR base({model, materials, meshes}, {transform})

ModelRender::ModelRender(Graphics::pModel model) : BASE_CTOR { this->model = model; }

ModelRender::ModelRender() : BASE_CTOR {}

void ModelRender::onInit() {
    auto g_mgr = getEngine()->getGraphicsManager();

    uint n = model->getSubMeshesCount();
    for (uint i = 0; i < n; i++) {
        meshes.push_back(
            Graphics::RenderingMesh(g_mgr, model->getSubMesh(i), materials[i]));
    }
}

void ModelRender::setModel(Graphics::pModel model) { this->model = model; }

Graphics::pModel ModelRender::getModel() { return model; }

void ModelRender::setMaterial(Graphics::pMaterial material, uint subMeshIdx) {
    materials[subMeshIdx] = material;
}

NSP_COMPONENTS_END
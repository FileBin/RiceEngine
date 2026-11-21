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
    // One slot per submesh, holes included: resetSubMesh(index) must find the
    // RenderingMesh of *that* submesh, and a submesh can be skipped here (no
    // material, empty mesh) without shifting every later index.
    rendering_meshes.assign(n, nullptr);
    for (uint i = 0; i < n; i++) {
        auto mat = materials[i];
        auto mesh = model->getSubMesh(i);
        if (mat && !mesh->isEmpty()) {
            auto rm = new_ptr<Graphics::RenderingMesh>(
                g_mgr, mesh, mat, transform);
            rendering_meshes[i] = rm;
            getSceneRender()->registerMesh(rm);
        }
    }
}

void ModelRender::onDisable() {
    for (auto &rm : rendering_meshes) {
        if (rm)
            getSceneRender()->unregisterMesh(rm);
    }
    rendering_meshes.clear();
}

void ModelRender::setModel(ptr<Model> model) {
    this->model = model;
    updateRenderData();
}

void ModelRender::updateRenderData() {
    // Only the CPU-side material list: the live RenderingMeshes (and the
    // geometry they hold) are rebuilt on the next disable/enable cycle.
    materials.resize(model->getSubMeshesCount());
}

ptr<Model> ModelRender::getModel() { return model; }

ptr<Material> ModelRender::getMaterial(size_t subMeshIdx) const {
    return subMeshIdx < materials.size() ? materials[subMeshIdx] : nullptr;
}

ptr<RenderingMesh> ModelRender::getRenderingMesh(size_t index) const {
    return index < rendering_meshes.size() ? rendering_meshes[index] : nullptr;
}

void ModelRender::setMaterial(ptr<Material> material, uint subMeshIdx) {
    materials[subMeshIdx] = material;
}

void ModelRender::resetSubMesh(ptr<Mesh> mesh, size_t index) {
    if (!model)
        THROW_EXCEPTION("ModelRender::resetSubMesh: no model set");
    if (index >= model->getSubMeshesCount())
        THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
    if (!mesh)
        THROW_NULL_PTR_EXCEPTION(mesh.get());
    if (mesh->isEmpty() || mesh->indexBuffer.empty())
        THROW_EXCEPTION("ModelRender::resetSubMesh: the new mesh is empty");
    if (!isActive())
        THROW_EXCEPTION("ModelRender::resetSubMesh: the component is disabled; set the "
                        "model instead and enable the component");

    // CPU side first, so the model and the GPU agree on what the submesh is.
    model->setSubMesh(mesh, index);
    if (rendering_meshes.size() < model->getSubMeshesCount())
        rendering_meshes.resize(model->getSubMeshesCount(), nullptr);

    if (auto rm = rendering_meshes[index]) {
        // Same buffers, new data and a forced command re-record. The material
        // of the RenderingMesh is untouched, so the mesh stays in its queue.
        rm->resetMesh(mesh);
        return;
    }

    // The submesh had no RenderingMesh when the component was enabled (it was
    // empty, or had no material yet). Build and register it now.
    ptr<Material> mat = index < materials.size() ? materials[index] : nullptr;
    if (!mat)
        THROW_EXCEPTION("ModelRender::resetSubMesh: submesh has no material");
    auto transform = getObject()->getComponent<Transform>();
    if (!transform)
        THROW_EXCEPTION("Object has no Transform component!");

    auto rm = new_ptr<Graphics::RenderingMesh>(
        getClientEngine()->getGraphicsManager(), mesh, mat, transform);
    rendering_meshes[index] = rm;
    getSceneRender()->registerMesh(rm);
}

NSP_COMPONENTS_END

#include "Rice/Engine/Log.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/LightData.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/GL/ModelData.hpp"
#include "Rice/GL/UniformBuffer.hpp"
#include "Rice/GL/VertexLayout.hpp"
#include "Rice/Math/Matrixes.hpp"
#include "Rice/namespaces.h"
#include "pch.h"
#include <Rice/Math/Vectors.hpp>

#include <Rice/GL/RenderingMesh.hpp>

using namespace Rice::Graphics;

NSP_GL_BEGIN

RenderingMesh::RenderingMesh(ptr<GraphicsManager> g_mgr, ptr<Mesh> mesh, ptr<Material> mat,
                             ptr<Components::Transform> t) {
    graphics_manager = g_mgr;
    material = mat;
    orig = mesh;
    transform = t;

    constBuffer = new_ptr<UniformBuffer>(g_mgr, sizeof(ModelData));
    constBuffer->updateData<ModelData>({});
    vertexBuffer = new_ptr<VertexBuffer>(g_mgr, mesh->vertexBuffer);
    indexBuffer = new_ptr<IndexBuffer>(g_mgr, mesh->indexBuffer);

    // auto cmd = new_ptr<CommandBuffer>(g_mgr, true);

    // auto ub = material->getUniformBuffer();

    // cmd->setActiveShader(material->getShader());
    // cmd->bindVertexBuffer(vertexBuffer);
    // cmd->bindIndexBuffer(indexBuffer);
    // cmd->bindUniformBuffer(constBuffer, 0);
    //  if (ub) {
    //      cmd->bindUniformBuffer(ub, 1);
    //  }
    // cmd->drawIndexed(indexBuffer);
    // cmd->buildAll();
}

void RenderingMesh::updateCmdBuffer() {
    // TODO recreate when vertex count changes
    // cmdBuffer->update();
}

void RenderingMesh::resetMesh(ptr<Mesh> mesh) {
    if (!mesh)
        THROW_NULL_PTR_EXCEPTION(mesh.get());
    if (mesh->vertexBuffer.count() == 0 || mesh->indexBuffer.empty())
        THROW_EXCEPTION("RenderingMesh::resetMesh: the new mesh is empty (remove the "
                        "submesh from the model instead)");

    // The buffers are freed below, so no GPU work may still reference them:
    // the previous frame's submission has to be complete (uploads and draws of
    // frame N-1 are submitted before its render fence is signalled). Queued but
    // not yet submitted staging copies are dropped by Buffer_API_Data::free().
    graphics_manager->sync();

    orig = mesh;
    // Both resets free and re-allocate the backing buffer, so the vertex/index
    // count may change freely. Keeps the same Buffer objects, which is what the
    // recorded commands hold on to.
    vertexBuffer->reset(mesh->vertexBuffer);
    indexBuffer->reset(mesh->indexBuffer);
    // The old DrawIndexed command still carries the old count; SceneRender
    // re-records every command buffer on the next frame while this is set.
    cmd_dirty = true;
}

void RenderingMesh::updateConstBuffer(const Matrix4x4 &view, const Matrix4x4 &proj) {
    ModelData data;
    auto world = transform->getTransformationMatrix();
    data.world = world;
    data.world_view = world * view;
    data.projection = proj;
    constBuffer->updateData(data);
}

void RenderingMesh::addCommands(ptr<Graphics::CommandBuffer> cmd) {
    cmd->bindVertexBuffer(vertexBuffer);
    cmd->bindIndexBuffer(indexBuffer);
    cmd->bindUniformBuffer(constBuffer, 0);
    cmd->drawIndexed(indexBuffer);
}

void RenderingMesh::addShadowCommands(ptr<Graphics::CommandBuffer> cmd, ptr<UniformBuffer> light,
                                      uint light_binding) {
    cmd->bindVertexBuffer(vertexBuffer);
    cmd->bindIndexBuffer(indexBuffer);
    // Same ModelData object the scene draw uses: its `world` is refreshed every
    // frame in updateConstBuffer(), so the map follows the object for free.
    cmd->bindUniformBuffer(constBuffer, LightingBindings::kMeshData);
    cmd->bindUniformBuffer(light, light_binding);
    cmd->drawIndexed(indexBuffer);
}

NSP_GL_END
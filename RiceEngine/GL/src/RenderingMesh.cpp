#include "Rice/Engine/Log.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/GL/ModelData.hpp"
#include "Rice/GL/UniformBuffer.hpp"
#include "Rice/GL/VertexLayout.hpp"
#include "Rice/Math/Matrixes.hpp"
#include "Rice/Math/Vectors/Vector3.hpp"
#include "Rice/namespaces.h"
#include "pch.h"

#include <Rice/GL/RenderingMesh.hpp>

using namespace Rice::Graphics;

NSP_GL_BEGIN

RenderingMesh::RenderingMesh(ptr<GraphicsManager> g_mgr, ptr<Mesh> mesh, ptr<Material> mat,
                             ptr<Components::Transform> t) {
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

void RenderingMesh::updateConstBuffer(Matrix4x4 view, Matrix4x4 proj) {
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

NSP_GL_END
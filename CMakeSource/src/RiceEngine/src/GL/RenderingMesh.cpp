#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/GL/ModelData.hpp"
#include "Rice/GL/VertexLayout.hpp"
#include "Rice/Math/Matrixes.hpp"
#include "Rice/namespaces.h"
#include "pch.h"

#include <Rice/GL/RenderingMesh.hpp>

NSP_GL_BEGIN

RenderingMesh::RenderingMesh(pGraphicsManager g_mgr, pMesh mesh,
                             pMaterial mat) {
    material = mat;
    orig = mesh;

    constBuffer = new_ref<UniformBuffer>(g_mgr, sizeof(ModelData));
    constBuffer->setShader(material->getShader());
    constBuffer->setBinding(0, sizeof(ModelData));
    constBuffer->updateDataAll<ModelData>({});
    vertexBuffer = new_ref<VertexBuffer, pGraphicsManager, VertexList &>(
        g_mgr, *mesh->vertexBuffer);
    indexBuffer = new_ref<IndexBuffer>(g_mgr, mesh->indexBuffer);

    pCommandBuffer cmd = new_ref<CommandBuffer>(g_mgr);

    auto ub = material->getUniformBuffer();
    if (ub.isNotNull())
        cmd->bindUniformBuffer(ub);

    cmd->setActiveShader(material->getShader());
    cmd->bindVertexBuffer(vertexBuffer);
    cmd->bindIndexBuffer(indexBuffer);

    // cmd->pushConstants<VertConstData>(const_data, test_shader);
    cmd->bindUniformBuffer(constBuffer);
    cmd->drawIndexed(indexBuffer);
    cmd->buildAll();

    cmdBuffer = cmd;
}

void RenderingMesh::updateConstBuffer(Matrix4x4f view, Matrix4x4f proj) {
    ModelData data;
    data.world = transform->getTransformationMatrix();
    data.view = view;
    data.projection = proj;
    constBuffer->updateData(data);
}

NSP_GL_END
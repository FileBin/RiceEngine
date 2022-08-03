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

NSP_GL_BEGIN

RenderingMesh::RenderingMesh(ptr<GraphicsManager> g_mgr, ptr<Mesh> mesh,
                             ptr<Material> mat, ptr<Components::Transform> t) {
    material = mat;
    orig = mesh;
    transform = t;

    constBuffer = new_ptr<UniformBuffer>(g_mgr, sizeof(ModelData));
    constBuffer->setShader(material->getShader());
    constBuffer->setBinding(0, sizeof(ModelData));
    constBuffer->build();
    constBuffer->updateDataAll<ModelData>({});
    vertexBuffer = new_ptr<VertexBuffer>(g_mgr, mesh->vertexBuffer);
    indexBuffer = new_ptr<IndexBuffer>(g_mgr, mesh->indexBuffer);

    auto cmd = new_ptr<CommandBuffer>(g_mgr);

    auto ub = material->getUniformBuffer();
    if (ub)
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

void RenderingMesh::updateCmdBuffer() {
    // TODO update cmdBuffer
    Log::log(Log::Warning, "RenderingMesh::updateCmdBuffer() not implemented");
}

void RenderingMesh::updateConstBuffer(Matrix4x4f view, Matrix4x4f proj) {
    ModelData data;
    data.world = Matrix4x4::translation({0,0,4});
    data.view = view;
    data.projection = proj;
    constBuffer->updateData(data); // test
}

NSP_GL_END
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/namespaces.h"
#include "pch.h"

#include <Rice/GL/RenderingMesh.hpp>

NSP_GL_BEGIN

pCommandBuffer RenderingMesh::createCmdBuffer(pGraphicsManager g_mgr) {
    pCommandBuffer cmd = new_ref<CommandBuffer>(g_mgr);

    auto ub = mat->getUniformBuffer();
    if (ub.isNotNull())
        cmd->bindUniformBuffer(ub);

    cmd->setActiveShader(mat->getShader());
    cmd->bindVertexBuffer(vertexBuffer);
    cmd->bindIndexBuffer(indexBuffer);

    // cmd->pushConstants<VertConstData>(const_data, test_shader);
    cmd->bindUniformBuffer(constBuffer);
    cmd->drawIndexed(indexBuffer);
    cmd->buildAll();
}

NSP_GL_END
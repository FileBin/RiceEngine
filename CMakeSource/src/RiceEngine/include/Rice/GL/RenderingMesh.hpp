#include "../stdafx.hpp"
#include "BetterCpp/Objects/PtrTypes.hpp"
#include "Rice/namespaces.h"

NSP_GL_BEGIN

class RenderingMesh;
typedef RefPtr<RenderingMesh> pRenderingMesh;

NSP_GL_END

#pragma once

#include "BetterCpp/Interfaces/Interfaces.hpp"
#include "GraphicsManager.hpp"
#include "IndexBuffer.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "Rice/Math/Matrixes.hpp"
#include "UniformBuffer.hpp"
#include "VertexBuffer.hpp"

#include "../Scene/Components/Transform.hpp"

NSP_GL_BEGIN

class RenderingMesh : public IRegistrable {
  private:
    size_t idx = std::numeric_limits<size_t>::max();

    Components::pTransform transform;
    pMesh orig;
    pMaterial material;
    pUniformBuffer constBuffer = nullptr;
    pIndexBuffer indexBuffer = nullptr;
    pVertexBuffer vertexBuffer = nullptr;
    pCommandBuffer cmdBuffer;

  public:
    RenderingMesh(pGraphicsManager graphics_manager, pMesh mesh, pMaterial material);
    Components::pTransform getTransform() { return transform; }
    pMaterial getMaterial() { return material; }
    pCommandBuffer getCmd() { return cmdBuffer; }

    void Register(size_t i) { idx = i; }

    size_t Unregister() {
        auto i = idx;
        idx = std::numeric_limits<size_t>::max();
        return i;
    }

    void updateCmdBuffer();
    void updateConstBuffer(Matrix4x4f view, Matrix4x4f projection);

    bool isTransparent() {
        return material->renderType == RenderType::Transparent;
    }
};

NSP_GL_END

#pragma once

#include "../stdafx.hpp"

#include "BetterCpp/Interfaces/Interfaces.hpp"
#include "GraphicsManager.hpp"
#include "IndexBuffer.hpp"
#include "Model.hpp"
#include "UniformBuffer.hpp"
#include "VertexBuffer.hpp"
#include "Material.hpp"

#include "../Scene/Components/Transform.hpp"

NSP_GL_BEGIN

class RenderingMesh : public IRegistrable {
  public:
    size_t idx = std::numeric_limits<uint>::max();

    Components::pTransform transform;
    pMesh orig;
    pMaterial mat;
    pUniformBuffer constBuffer = nullptr;
    pIndexBuffer indexBuffer = nullptr;
    pVertexBuffer vertexBuffer = nullptr;

    void Register(size_t i) { idx = i; }

    size_t Unregister() {
        auto i = idx;
        idx = std::numeric_limits<size_t>::max();
        return i;
    }

    pCommandBuffer createCmdBuffer(pGraphicsManager graphics_manager);

    bool IsTransparent() { return mat->renderType == RenderType::Transparent; }
};

NSP_GL_END

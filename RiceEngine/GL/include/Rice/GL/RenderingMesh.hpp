#include "stdafx.hpp"

NSP_GL_BEGIN

class RenderingMesh;

NSP_GL_END

#pragma once

#include "GraphicsManager.hpp"
#include "IndexBuffer.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "Rice/Math/Matrixes.hpp"
#include "UniformBuffer.hpp"
#include "VertexBuffer.hpp"

#include "Rice/Scene/Components/Transform.hpp"

NSP_GL_BEGIN

class RenderingMesh : public IRegistrable {
  private:
    size_t idx = std::numeric_limits<size_t>::max();

    ptr<Components::Transform> transform;
    ptr<Mesh> orig;
    ptr<Material> material;
    ptr<UniformBuffer> constBuffer = nullptr;
    ptr<IndexBuffer> indexBuffer = nullptr;
    ptr<VertexBuffer> vertexBuffer = nullptr;
    ptr<CommandBuffer> cmdBuffer;

  public:
    RenderingMesh(ptr<GraphicsManager> graphics_manager, ptr<Mesh> mesh, ptr<Material> material);
    ptr<Components::Transform> getTransform() { return transform; }
    ptr<Material> getMaterial() { return material; }
    ptr<CommandBuffer> getCmd() { return cmdBuffer; }

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

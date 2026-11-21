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

    ptr<GraphicsManager> graphics_manager;
    ptr<Components::Transform> transform;
    ptr<Mesh> orig;
    ptr<Material> material;
    ptr<UniformBuffer> constBuffer = nullptr;
    ptr<IndexBuffer> indexBuffer = nullptr;
    ptr<VertexBuffer> vertexBuffer = nullptr;
    // Set by resetMesh(), consumed by SceneRender once per frame. The index
    // count of a DrawIndexed command is baked when the command buffer is built,
    // so a geometry change of a different size needs a re-record.
    bool cmd_dirty = false;
    // ptr<CommandBuffer> cmdBuffer;

  public:
    RenderingMesh(ptr<GraphicsManager> graphics_manager, ptr<Mesh> mesh,
                  ptr<Material> material, ptr<Components::Transform> transform);
    ptr<Components::Transform> getTransform() { return transform; }
    ptr<Material> getMaterial() { return material; }
    // ptr<CommandBuffer> getCmd() { return cmdBuffer; }

    /**
     * Replaces the geometry of this rendering mesh with `mesh` — any vertex and
     * index count. Both GPU buffers are rebuilt through VertexBuffer::reset /
     * IndexBuffer::reset, and the draw command is marked to be recorded again
     * (SceneRender picks the flag up before it rebuilds the command buffers, so
     * the new index count is baked into DrawIndexed). The material binding is
     * untouched: batching is keyed on the material, geometry changes must not
     * move the mesh between queues.
     *
     * `mesh` must be non-empty; a submesh that becomes empty has to be removed
     * from the model instead. Call this between frames (from onUpdate /
     * onPreUpdate of a script), never while a frame is being recorded or drawn:
     * the previous buffers are freed immediately.
     */
    void resetMesh(ptr<Mesh> mesh);

    /** Vertex storage, for constant-size partial uploads (Path A in the API
     * reference): getVertexBuffer()->updateVertices(list, byteOffset). */
    ptr<VertexBuffer> getVertexBuffer() { return vertexBuffer; }
    /** Index storage, for constant-size partial uploads: the offset of
     * updateIndices() is an element index, not a byte offset. */
    ptr<IndexBuffer> getIndexBuffer() { return indexBuffer; }

    /** The pending re-record flag, without consuming it. */
    bool isCmdDirty() const { return cmd_dirty; }

    /** True exactly once after resetMesh(). SceneRender consumes it to force a
     * command-buffer re-record; game code has no reason to call it. */
    bool consumeCmdDirty() {
        bool d = cmd_dirty;
        cmd_dirty = false;
        return d;
    }

    void addCommands(ptr<Graphics::CommandBuffer> cmd);

    void Register(size_t i) { idx = i; }

    size_t Unregister() {
        auto i = idx;
        idx = std::numeric_limits<size_t>::max();
        return i;
    }

    void updateCmdBuffer();
    void updateConstBuffer(const Matrix4x4 &view, const Matrix4x4 &projection);

    bool isTransparent() { return material->renderType == RenderType::Transparent; }
};

NSP_GL_END

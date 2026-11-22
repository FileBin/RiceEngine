#include "../../stdafx.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/Scene/Components/Transform.hpp"

NSP_COMPONENTS_BEGIN
class ModelRender;
NSP_COMPONENTS_END

#pragma once

#include "Rice/GL/RenderingMesh.hpp"

NSP_COMPONENTS_BEGIN

class ModelRender : public virtual Components::Component {
  private:
    void onEnable() override;
    void onDisable() override;

  public:
    ModelRender();
    ModelRender(ptr<Graphics::Model> model);

    void updateRenderData();
    // Takes effect immediately for the CPU-side material list; for the GPU side
    // it is the running RenderingMesh that owns the material, so a material set
    // after onEnable() is applied on the next disable/enable cycle. Neither
    // setMaterial() nor setModel() touches the uploaded geometry of the live
    // submeshes, so a resetSubMesh() upload survives them.
    void setMaterial(ptr<Graphics::Material> material, uint subMeshIdx);
    void setModel(ptr<Graphics::Model> model);
    ptr<Graphics::Model> getModel();
    ptr<Graphics::Material> getMaterial(size_t subMeshIdx) const;

    /* The live RenderingMesh of submesh `index`, or nullptr if that submesh has
     * none (no material, empty mesh, or the component is disabled). This is the
     * handle for Path A streaming: getVertexBuffer()/getIndexBuffer() allow
     * constant-size partial uploads without re-recording. */
    ptr<Graphics::RenderingMesh> getRenderingMesh(size_t index) const;

    /**
     * Replaces the geometry of submesh `index` of a live (enabled) component:
     * the CPU model of that submesh is swapped, and the submesh's RenderingMesh
     * gets the new vertex/index buffers through resetMesh(), which also forces
     * the recorded draw command to be recorded again with the new index count.
     * The material of the submesh is left bound, and the mesh stays in the same
     * render queue.
     *
     * - `index` must address an existing submesh of the current model;
     * - the component must be enabled (a disabled component has no
     *   RenderingMesh to update — set the model and let onEnable build it);
     * - `mesh` must be non-empty; to drop a submesh, disable the component.
     *
     * Throws Util::Exception (index out of range / not enabled / empty mesh)
     * instead of silently drawing stale geometry. Call it between frames, from
     * onUpdate/onPreUpdate, never while a frame is being recorded.
     */
    void resetSubMesh(ptr<Graphics::Mesh> mesh, size_t index);

  private:
    wptr<Transform> transform;
    uint idx;
    ptr<Graphics::Model> model;
    vec<ptr<Graphics::Material>> materials;
    vec<ptr<Graphics::RenderingMesh>> rendering_meshes;
};
NSP_COMPONENTS_END

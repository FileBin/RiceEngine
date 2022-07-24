#include "../../stdafx.hpp"

NSP_COMPONENTS_BEGIN
class PTR_PROTO(ModelRender);
NSP_COMPONENTS_END

#pragma once

#include "../../GL/RenderingMesh.hpp"
#include "../Component.hpp"

NSP_COMPONENTS_BEGIN

class ModelRender : public Component {
  private:
    void onInit();

    void onEnable();
    void onDisable();

  public:
    void setMaterial(Graphics::pMaterial material, uint subMeshIdx);
    void setModel(Graphics::pModel model);
    Graphics::pModel getModel();
    Graphics::pMaterial getMaterial(size_t subMeshIdx) const;

  private:
    pTransform transform;
    uint idx;
    Graphics::pModel model;
    vec<Graphics::RenderingMesh> meshes;
};
NSP_COMPONENTS_END

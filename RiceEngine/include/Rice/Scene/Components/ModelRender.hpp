#include "../../stdafx.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/Scene/Components/Transform.hpp"

NSP_COMPONENTS_BEGIN
class PTR_PROTO(ModelRender);
NSP_COMPONENTS_END

#pragma once

#include "../../GL/RenderingMesh.hpp"
#include "../PackableComponent.hpp"

NSP_COMPONENTS_BEGIN

class ModelRender : public PackableComponent<ModelRender, RefTuple<Graphics::pModel, vec<Graphics::pMaterial>, vec<Graphics::RenderingMesh>>, RefTuple<pTransform>> {
  private:
    void onInit() override;

   //TODO void onEnable() override;
   //TODO void onDisable() override;

  public:
    ModelRender();
    ModelRender(Graphics::pModel model);

    void setMaterial(Graphics::pMaterial material, uint subMeshIdx);
    void setModel(Graphics::pModel model);
    Graphics::pModel getModel();
    Graphics::pMaterial getMaterial(size_t subMeshIdx) const;

  private:
    pTransform transform;
    uint idx;
    Graphics::pModel model;
    vec<Graphics::pMaterial> materials;
    vec<Graphics::RenderingMesh> meshes;
};
NSP_COMPONENTS_END

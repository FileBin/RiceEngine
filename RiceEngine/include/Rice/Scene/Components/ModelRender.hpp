#include "../../stdafx.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/Scene/Components/Transform.hpp"

NSP_COMPONENTS_BEGIN
class ModelRender;
NSP_COMPONENTS_END

#pragma once

#include "../PackableComponent.hpp"
#include "Rice/GL/RenderingMesh.hpp"

NSP_COMPONENTS_BEGIN

class ModelRender
    : virtual public PackableComponentT<ModelRender> {
  private:
    void onInit() override;

    // TODO void onEnable() override;
    // TODO void onDisable() override;

  public:
    ModelRender();
    ModelRender(ptr<Graphics::Model> model);

    void setMaterial(ptr<Graphics::Material> material, uint subMeshIdx);
    void setModel(ptr<Graphics::Model> model);
    ptr<Graphics::Model> getModel();
    ptr<Graphics::Material> getMaterial(size_t subMeshIdx) const;

  private:
    wptr<Transform> transform;
    uint idx;
    ptr<Graphics::Model> model;
    vec<ptr<Graphics::Material>> materials;
    vec<Graphics::RenderingMesh> meshes;
};
NSP_COMPONENTS_END

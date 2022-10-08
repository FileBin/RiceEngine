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
    void setMaterial(ptr<Graphics::Material> material, uint subMeshIdx);
    void setModel(ptr<Graphics::Model> model);
    ptr<Graphics::Model> getModel();
    ptr<Graphics::Material> getMaterial(size_t subMeshIdx) const;

  private:
    wptr<Transform> transform;
    uint idx;
    ptr<Graphics::Model> model;
    vec<ptr<Graphics::Material>> materials;
    vec<ptr<Graphics::RenderingMesh>> rendering_meshes;
};
NSP_COMPONENTS_END

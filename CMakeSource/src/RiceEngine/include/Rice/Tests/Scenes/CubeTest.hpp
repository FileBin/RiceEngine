#include "../../Scene/Scene.hpp"
#include "../../stdafx.hpp"
#include "BetterCpp/Functions.hpp"
#include "Rice/GL/Mesh.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/Scene/Components/Camera.hpp"
#include "Rice/Scene/Components/ModelRender.hpp"
#include "Rice/Scene/Components/Transform.hpp"

#pragma once

NSP_TESTS_BEGIN

class CubeTestScene : public Scene {
    void init() override {
        auto cam_obj = createEmpty(); 
        auto cam_comp = new_ref<Components::Camera>();
        cam_obj->addComponent(cam_comp);

        setActiveCamera(cam_comp);

        auto cube_obj = createEmpty();
        auto cube_transform = new_ref<Components::Transform>();

        cube_transform->setPosition({0,0,4});

        auto cube_render = new_ref<Components::ModelRender>();

        cube_obj->addComponent(cube_transform);
        cube_obj->addComponent(cube_render);

        auto cube_model = new_ref<Graphics::Model>();

        auto cube_mesh = new_ref<Graphics::Mesh>();

        cube_mesh->combine(Graphics::Mesh::quad);

        cube_model->setSubMeshesCount(1);
        cube_model->setSubMesh(cube_mesh, 0);
        cube_render->setModel(cube_model);  
    }
};

NSP_TESTS_END
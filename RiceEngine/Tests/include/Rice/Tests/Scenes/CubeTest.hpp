#include "../../GL/Mesh.hpp"
#include "../../GL/Model.hpp"
#include "../../Scene/Components/Camera.hpp"
#include "../../Scene/Components/ModelRender.hpp"
#include "../../Scene/Components/Transform.hpp"
#include "../../Scene/Scene.hpp"
#include "../../stdafx.hpp"
#include "Rice/Engine/Engine.hpp"
#include "Rice/GL/Shader.hpp"

#pragma once

NSP_TESTS_BEGIN

class CubeTestScene : public Scene {
    void init() override {
        auto en = getEngine();

        auto cam_obj = createEmpty();
        auto cam_comp = new_ref<Components::Camera>();
        cam_obj->addComponent(cam_comp);

        setActiveCamera(cam_comp);

        auto cube_obj = createEmpty();
        auto cube_transform = new_ref<Components::Transform>();

        cube_transform->setPosition({0, 0, 4});

        auto cube_render = new_ref<Components::ModelRender>();

        cube_obj->addComponent(cube_transform);
        cube_obj->addComponent(cube_render);

        auto cube_model = new_ref<Graphics::Model>();

        auto cube_mesh = Graphics::Mesh::quad.clone();

        cube_model->setSubMeshesCount(1);
        cube_model->setSubMesh(cube_mesh, 0);
        cube_render->setModel(cube_model);

        auto cube_material = en->getOrCreateMaterial(
            "cube_material", [](pEngine en) -> Graphics::pShader {
                auto shader = en->getOrCreateShader(
                    "solid", [](Graphics::pShader shader) -> void {
                        shader->loadShader("shaders/solid.vert.spv",
                                           Graphics::Shader::Vertex);
                        shader->loadShader("shaders/solid.frag.spv",
                                           Graphics::Shader::Fragment);
                    });
                return shader;
            });

        cube_render->setMaterial(cube_material, 0);
    }
};

NSP_TESTS_END
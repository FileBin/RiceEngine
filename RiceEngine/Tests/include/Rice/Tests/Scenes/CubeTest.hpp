#include "Rice/Engine/Engine.hpp"
#include "Rice/GL/Mesh.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/Scene/Components/Camera.hpp"
#include "Rice/Scene/Components/ModelRender.hpp"
#include "Rice/Scene/Components/Transform.hpp"
#include "Rice/Scene/Scene.hpp"
#include "Rice/defines.h"
#include "Rice/stdafx.hpp"

#pragma once

NSP_TESTS_BEGIN

class CubeTestScene : public virtual Scene {
    CubeTestScene() = default;

  public:
    static ptr<CubeTestScene> create() {
        return ptr<CubeTestScene>(new CubeTestScene());
    }

    void init() override {
        auto en = getEngine();

        auto cam_obj = createEnabled("Camera");
        auto cam_transform = new_ptr<Components::Transform>();
        cam_transform->setPosition({0, 0, -5});
        auto cam_comp = new_ptr<Components::Camera>();
        cam_obj->addComponent(cam_transform);
        cam_obj->addComponent(cam_comp);

        setActiveCamera(cam_comp);

        auto cube_obj = createEnabled("Cube");
        auto cube_transform = new_ptr<Components::Transform>();

        cube_transform->setPosition({0, 0, 0});

        auto cube_render = new_ptr<Components::ModelRender>();

        cube_obj->addComponent(cube_transform);
        cube_obj->addComponent(cube_render);

        auto cube_model = new_ptr<Graphics::Model>();

        auto cube_mesh = Graphics::Mesh::quad.clone();
        //cube_mesh->rotate(Quaternion::fromEulerAngles({0, 180, 0}));

        cube_model->setSubMeshesCount(1);
        cube_model->setSubMesh(cube_mesh, 0);
        cube_render->setModel(cube_model);

        auto cube_material = en->getOrCreateMaterial(
            "cube_material", [](ptr<Engine> en) -> ptr<Graphics::Shader> {
                auto shader = en->getOrCreateShader(
                    "solid", [](ptr<Graphics::Shader> shader) -> void {
                        shader->loadShader("shaders/solid.vert.spv",
                                           Graphics::Shader::Vertex);
                        shader->loadShader("shaders/solid.frag.spv",
                                           Graphics::Shader::Fragment);
                        shader->addUniformBuffer(0, Graphics::Shader::Vertex);
                        shader->setVertexStrideAndLayout<Graphics::Vertex>();
                        shader->build();
                    });
                return shader;
            });

        cube_render->setMaterial(cube_material, 0);
    }
};

NSP_TESTS_END
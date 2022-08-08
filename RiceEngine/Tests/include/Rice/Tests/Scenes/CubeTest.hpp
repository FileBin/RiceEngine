#undef RICE_SOURCE

#include "Rice/Engine.hpp"
#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Mesh.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/Scene/ClientScene.hpp"
#include "Rice/Scene/Components/Camera.hpp"
#include "Rice/Scene/Components/ModelRender.hpp"
#include "Rice/Scene/Components/Transform.hpp"
#include "Rice/Scene/Scene.hpp"
#include "Rice/Tests/Scripts/CameraMover.hpp"
#include "Rice/defines.h"

#pragma once

NSP_TESTS_BEGIN

class CubeTestScene : public virtual Scene {
    CubeTestScene() = default;

  public:
    static ptr<CubeTestScene> create() {
        return ptr<CubeTestScene>(new CubeTestScene());
    }

    void init() override {
        auto en = getClientEngine();
        auto ren = getSceneRender();

        auto cam_obj = createEnabled("Camera");
        auto cam_transform = new_ptr<Components::Transform>();
        cam_transform->setPosition({0, 0, -5});
        auto cam_comp = new_ptr<Components::Camera>();
        auto cam_mover = new_ptr<Tests::CameraMover>();

        cam_obj->addComponent(cam_transform);
        cam_obj->addComponent(cam_comp);
        cam_obj->addComponent(cam_mover);

        setActiveCamera(cam_comp);

        for (uint i = 0; i < 2; ++i) {

            auto cube_obj = createEnabled(fmt::format("Cube {}", i));
            auto cube_transform = new_ptr<Components::Transform>();

            cube_transform->setPosition({i * 2., 0, 0});

            auto cube_render = new_ptr<Components::ModelRender>();

            cube_obj->addComponent(cube_transform);
            cube_obj->addComponent(cube_render);

            auto cube_model = new_ptr<Graphics::Model>();

            auto cube_mesh = Graphics::Mesh::cube().clone();
            // cube_mesh->rotate(Quaternion::fromEulerAngles({0, 180, 0}));

            cube_model->setSubMeshesCount(1);
            cube_model->setSubMesh(cube_mesh, 0);
            cube_render->setModel(cube_model);

            auto cube_material = ren->getOrCreateMaterial(
                "cube_material", 
                [](ptr<SceneRender> ren) -> ptr<Graphics::Material> {
                    auto shader = ren->getOrCreateShader(
                        "solid", [](ptr<Graphics::Shader> shader) -> void {
                            shader->loadShader("shaders/default.vert.spv",
                                               Graphics::Shader::Vertex);
                            shader->loadShader("shaders/diffuse.frag.spv",
                                               Graphics::Shader::Fragment);
                            shader->addUniformBuffer(0,
                                                     Graphics::Shader::Vertex);
                            shader->addUniformBuffer(
                                1, Graphics::Shader::Fragment);
                            shader
                                ->setVertexStrideAndLayout<Graphics::Vertex>();
                            shader->build();
                        });
                    auto material = new_ptr<Graphics::Material>(
                        ren->getGraphicsManager(), shader);

                    material->initUniformBuffer<Vector4f>({0, 1, 0, 1});

                    return material;
                });

            cube_render->setMaterial(cube_material, 0);
        }
    }
};

NSP_TESTS_END
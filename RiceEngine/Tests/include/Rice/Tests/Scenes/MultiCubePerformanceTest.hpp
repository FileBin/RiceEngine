#include "Rice/Math/Vectors/Vector4f.hpp"
#include <tuple>
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

class MultiCubePerformanceTestScene : public virtual Scene {
    MultiCubePerformanceTestScene() = default;

  public:
    static ptr<MultiCubePerformanceTestScene> create() {
        return ptr<MultiCubePerformanceTestScene>(new MultiCubePerformanceTestScene());
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
        constexpr uint cx = 10;
        constexpr uint cy = 10;
        constexpr uint cz = 10;
        constexpr uint count = cx * cy * cz;
        for (uint i = 0; i < count; ++i) {
            dbl x = i % cx;
            dbl y = (i / cx) % cy;
            dbl z = floor(i / (cx * cy));
            Vector3 pos{x, y, z};
            pos *= 2.;
            pos -= Vector3{cx, cy, cz} * .5;
            auto cube_obj = createEnabled(fmt::format("Cube {}", i));
            auto cube_transform = new_ptr<Components::Transform>();

            cube_transform->setPosition(pos);

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
                fmt::format("cube_material ({})", i),
                [](ptr<SceneRender> ren) -> ptr<Graphics::Material> {
                    auto shader = ren->getOrCreateShader(
                        "solid", [](ptr<Graphics::Shader> shader) -> void {
                            shader->loadShader("shaders/default.vert.spv",
                                               Graphics::Shader::Vertex);
                            shader->loadShader("shaders/diffuse.frag.spv",
                                               Graphics::Shader::Fragment);
                            shader->addUniformBuffer(0, Graphics::Shader::Vertex);
                            shader->addUniformBuffer(1, Graphics::Shader::Fragment);
                            shader->setVertexStrideAndLayout<Graphics::Vertex>();
                            shader->build();
                        });
                    auto material =
                        new_ptr<Graphics::Material>(ren->getGraphicsManager(), shader);
                    Vector4f egst;
                    Vector4f color;
                    egst.x = Math::random();
                    egst.y = Math::random();
                    egst.z = Math::random();
                    egst.w = 1.f;
                    color.x = Math::random();
                    color.y = Math::random();
                    color.z = Math::random();
                    color.w = 1.f;
                    material->initUniformBuffer(std::make_tuple(color, egst));

                    return material;
                });

            cube_render->setMaterial(cube_material, 0);
        }
    }
};

NSP_TESTS_END
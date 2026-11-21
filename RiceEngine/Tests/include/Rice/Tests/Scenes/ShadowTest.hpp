/*
 * ShadowTest.hpp — scene for the directional-shadow smoke test.
 * See Tests/src/ShadowTest.cpp for what the run does.
 */

#include <SDL.h>

#include <cmath>
#include <cstdio>
#include <tuple>

#undef RICE_SOURCE

#include "Rice/Engine.hpp"
#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/GL/LightData.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Mesh.hpp"
#include "Rice/GL/Model.hpp"
#include "Rice/GL/RenderingMesh.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/GL/ShadowMap.hpp"
#include "Rice/Scene/ClientScene.hpp"
#include "Rice/Scene/Components/Camera.hpp"
#include "Rice/Scene/Components/DirectionalLight.hpp"
#include "Rice/Scene/Components/ModelRender.hpp"
#include "Rice/Scene/Components/MonoScript.hpp"
#include "Rice/Scene/Components/Transform.hpp"
#include "Rice/Scene/Scene.hpp"

#pragma once

NSP_TESTS_BEGIN

/* Collected by the scene, read by main() for the exit code. */
struct ShadowTestStats {
    inline static uint frames = 0;
    inline static uint failures = 0;
    // Number of meshes the last recorded casters pass drew, and the map size.
    inline static uint casters = 0;
    inline static uint map_size = 0;
    inline static uint filled_texels = 0;
    inline static bool light_found = false;

    static void fail(const char *what) {
        ++failures;
        Log::log(Log::Error, "ShadowTest: {}", what);
    }
};

/* Per-material uniform block of diffuse.frag: egst then color, in GLSL order.
 * (The older demos hand std::make_tuple(color, egst) to initUniformBuffer,
 * which is the reverse order; the struct keeps the two vec4s where the shader
 * expects them.) */
struct ShadowMaterialData {
    Vector4f egst;  // x emission, y glossiness, z specular
    Vector4f color;
};
static_assert(sizeof(ShadowMaterialData) == 32, "std140: two vec4s");

class ShadowTestScript : public virtual Components::MonoScript {
  public:
    static constexpr uint kFrames = 60; // frames the demo runs before it closes

    void onUpdate() override {
        // The first onUpdate runs before the first frame is drawn, so the
        // renderer has not created the shadow map yet. Start checking once one
        // frame has gone through SceneRender::draw().
        if (ShadowTestStats::frames >= 1) {
            check();
            if (ShadowTestStats::failures != 0) {
                finish();
                return;
            }
        }

        ++ShadowTestStats::frames;
        if (ShadowTestStats::frames >= kFrames)
            finish();
    }

  private:
    bool finished = false;

    void check() {
        auto ren = getSceneRender();
        auto map = ren->getShadowMap();

        // The light has to be found by the renderer, and the shadow pass has to
        // have been recorded with at least one caster in it: a map that was only
        // allocated (0 casters) is exactly the regression this checks for.
        if (!ren->getActiveLight())
            ShadowTestStats::fail("the renderer did not find the DirectionalLight");
        else
            ShadowTestStats::light_found = true;

        if (!map) {
            ShadowTestStats::fail("the scene has no shadow map");
            return;
        }

        ShadowTestStats::map_size = map->getSize();
        ShadowTestStats::casters = map->getCasterCount();
        if (map->getSize() != 2048)
            ShadowTestStats::fail("the shadow map is not 2048x2048");
        if (map->getCasterCount() == 0)
            ShadowTestStats::fail("no mesh was rendered into the shadow pass");
    }
    void finish() {
        if (finished)
            return;
        finished = true;

        // GPU-side proof, once at the end of the run: the map must actually
        // contain rasterized geometry, not merely be allocated. This reads the
        // depth image back, which stalls the GPU - exactly what we want here.
        auto map = getSceneRender()->getShadowMap();
        ShadowTestStats::filled_texels = map ? map->readFilledTexelCount() : 0;
        if (ShadowTestStats::filled_texels == 0)
            ShadowTestStats::fail("the shadow map contains no geometry");

        if (ShadowTestStats::failures == 0)
            std::printf("ShadowTest: %u frames, shadow map %ux%u, %u casters per frame, "
                        "%u filled texels\n",
                        ShadowTestStats::frames, ShadowTestStats::map_size,
                        ShadowTestStats::map_size, ShadowTestStats::casters,
                        ShadowTestStats::filled_texels);
        else
            std::printf("ShadowTest: finished with %u failure(s)\n", ShadowTestStats::failures);

        // Close the window the same way a window manager would, so the engine
        // takes its normal shutdown path and ClientCore::run() returns.
        auto window = getClientEngine()->getInputManager()->getWindow();
        SDL_Event event{};
        event.type = SDL_WINDOWEVENT;
        event.window.event = SDL_WINDOWEVENT_CLOSE;
        event.window.windowID = SDL_GetWindowID(window->getHandle().get());
        SDL_PushEvent(&event);
    }
};

class ShadowTestScene : public virtual Scene {
    ShadowTestScene() = default;

  public:
    static ptr<ShadowTestScene> create() { return ptr<ShadowTestScene>(new ShadowTestScene()); }

    void init() override {
        auto ren = getSceneRender();

        // --- camera: above and behind the scene, looking at the cubes -------
        auto cam_obj = createEnabled("Camera");
        auto cam_transform = new_ptr<Components::Transform>();
        const Vector3 cam_pos{0, 5, -12};
        cam_transform->setPosition(cam_pos);
        cam_transform->setRotation(Quaternion::lookAt(cam_pos, {0, 1, 0}));
        auto cam = new_ptr<Components::Camera>();
        cam->setFov(60);
        cam->setNearPlane(0.05);
        cam->setFarPlane(500);
        cam_obj->addComponent(cam_transform);
        cam_obj->addComponent(cam);
        setActiveCamera(cam);

        // --- the sun -------------------------------------------------------
        auto sun_obj = createEnabled("Sun");
        auto sun = new_ptr<Components::DirectionalLight>();
        // Rays travel down, towards +x and towards the camera (-z), so the
        // shadows fall in front of the objects instead of behind them.
        sun->setDirection({-0.35, -1, -0.55});
        sun->setAmbient({0.25f, 0.25f, 0.3f, 1.f});
        sun->setDiffuse({1.f, 1.f, 1.f, 1.f});
        sun->setShadowDistance(64);
        sun->setMapSize(2048);
        sun_obj->addComponent(sun);

        if (sun->getMapSize() != 2048 || sun->getShadowDistance() != 64)
            ShadowTestStats::fail("the DirectionalLight setters did not stick");
        if (std::abs(sun->getDirection().length() - 1.) > 1e-6)
            ShadowTestStats::fail("setDirection() did not store a unit vector");

        // --- one lit shader + two materials --------------------------------
        auto shader = ren->getOrCreateShader(
            "shadow_lit", [](ptr<Graphics::Shader> shader) -> void {
                // The lit variants: the same scene shaders compiled with
                // -DRICE_LIGHTING (light block at binding 2, shadow map at 3).
                shader->loadShader("shaders/default_lit.vert.spv", Graphics::Shader::Vertex);
                shader->loadShader("shaders/diffuse_lit.frag.spv", Graphics::Shader::Fragment);
                shader->addUniformBuffer(Graphics::LightingBindings::kMeshData,
                                         Graphics::Shader::Vertex);
                shader->addUniformBuffer(Graphics::LightingBindings::kMaterial,
                                         Graphics::Shader::Fragment);
                // Declares the scene light block (2) and the shadow sampler (3).
                shader->addLighting(Graphics::LightingBindings::kLightData,
                                    Graphics::LightingBindings::kShadowMap);
                shader->setVertexStrideAndLayout<Graphics::Vertex>();
                shader->build();
            });

        if (!shader->usesLighting())
            ShadowTestStats::fail("the shader did not register its lighting bindings");

        auto make_material = [&](const char *name, Vector4f color) {
            char full_name[64];
            std::snprintf(full_name, sizeof(full_name), "shadow_%s", name);
            return ren->getOrCreateMaterial(
                full_name, [color](ptr<SceneRender> ren) -> ptr<Graphics::Material> {
                    auto shader = ren->getShader("shadow_lit");
                    auto material =
                        new_ptr<Graphics::Material>(ren->getGraphicsManager(), shader);
                    material->initUniformBuffer(
                        ShadowMaterialData{{0.f, 32.f, 1.f, 0.f}, color});
                    return material;
                });
        };

        auto ground_material = make_material("ground", {0.35f, 0.55f, 0.3f, 1.f});
        auto cube_material = make_material("cube", {0.85f, 0.65f, 0.25f, 1.f});

        // --- ground: a flat box, so both faces exist for the depth pass -----
        auto ground_mesh = Graphics::Mesh::cube().clone();
        ground_mesh->scale({40, .2, 40});
        add_renderable("Ground", ground_mesh, ground_material, {0, -.1, 0});

        // --- four cubes standing on it -------------------------------------
        const Vector3 cube_positions[] = {{-3, .5, 0}, {0, .5, 0}, {3, .5, 0}, {1.5, 1.5, 2.5}};
        for (uint i = 0; i < 4; ++i) {
            auto mesh = Graphics::Mesh::cube().clone();
            add_renderable(fmt::format("Cube {}", i), mesh, cube_material, cube_positions[i]);
        }

        // --- the script that drives and checks the run ----------------------
        auto driver = createEnabled("ShadowTestDriver");
        driver->addComponent(new_ptr<ShadowTestScript>());
    }

  private:
    void add_renderable(const String &name, ptr<Graphics::Mesh> mesh,
                        ptr<Graphics::Material> material, Vector3 position) {
        auto obj = createEnabled(name);
        auto transform = new_ptr<Components::Transform>();
        transform->setPosition(position);
        auto render = new_ptr<Components::ModelRender>();
        obj->addComponent(transform);
        obj->addComponent(render);

        auto model = new_ptr<Graphics::Model>();
        model->setSubMeshesCount(1);
        model->setSubMesh(mesh, 0);
        render->setModel(model);
        render->setMaterial(material, 0);
    }
};

NSP_TESTS_END

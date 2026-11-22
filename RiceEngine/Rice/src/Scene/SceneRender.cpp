#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/LightData.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/Util/defines.hpp"
#include "pch.h"

#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/Engine/Log.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/GL/ShadowMap.hpp"
#include "Rice/Scene/ClientScene.hpp"
#include "Rice/Scene/Components/DirectionalLight.hpp"
#include "Rice/Scene/Object.hpp"
#include "Rice/Scene/Scene.hpp"
#include <Rice/GL/RenderingMesh.hpp>
#include <Rice/Scene/SceneRender.hpp>

#include <algorithm>

NSP_ENGINE_BEGIN

SceneRender::SceneRender(ptr<ClientScene> scene, ptr<ClientEngine> engine)
    : scene(scene), engine(engine) {
    main_cmd = new_ptr<Graphics::CommandBuffer>(engine->getGraphicsManager());
    clear_cmd = new_ptr<Graphics::CommandBuffer>(engine->getGraphicsManager(), true);
    clear_cmd->clearRenderTarget({0.1, 0.3, 0.7}, 1);
    clear_cmd->buildAll();
    updateCmd();
}

SceneRender::ShaderQueue::ShaderQueue(ptr<Graphics::Shader> shader,
                                      ptr<Graphics::GraphicsManager> mgr)
    : shader(shader) {
    cmd = new_ptr<Graphics::CommandBuffer>(mgr, true);
}

SceneRender::MaterialQueue::MaterialQueue(ptr<Graphics::Material> mat) : material(mat) {}

void SceneRender::MaterialQueue::addCommands(ptr<Graphics::CommandBuffer> cmd) {
    auto ub = material->getUniformBuffer();
    if (ub) {
        cmd->bindUniformBuffer(ub, 1);
    }

    auto mc = mesh_collection.getCollectionWithGaps();
    for (auto ptr : mc) {
        if (ptr) {
            ptr->addCommands(cmd);
        }
    }
}

void SceneRender::ShaderQueue::updateCmd() {
    cmd->clear();
    cmd->setActiveShader(shader);

    // A shader built with Shader::addLighting() declares binding 2 (light block)
    // and 3 (shadow map); every draw of this queue has to bind them or the
    // descriptor set is incomplete. Scenes without lighting resources (or
    // shaders without addLighting()) skip this and keep the old 0/1 layout.
    if (shader->usesLighting() && light_buffer && shadow_map) {
        cmd->bindUniformBuffer(light_buffer, shader->getLightBinding());
        cmd->bindShadowMap(shadow_map, shader->getShadowBinding());
    }

    // Material order inside the queue: opaque first, blended last, stable inside
    // each group (the materials map itself is ordered by pointer, so the result
    // is deterministic). A queue with one material - the usual case - records
    // exactly the commands it recorded before.
    vec<MaterialQueue *> ordered;
    ordered.reserve(materials.size());
    for (auto &mq : materials)
        ordered.push_back(&mq.second);
    std::stable_sort(ordered.begin(), ordered.end(),
                     [](const MaterialQueue *a, const MaterialQueue *b) {
                         return int(a->isTransparent()) < int(b->isTransparent());
                     });

    for (MaterialQueue *mq : ordered)
        mq->addCommands(cmd);

    cmd->buildAll();
}

void SceneRender::registerMesh(ptr<Graphics::RenderingMesh> mesh) {
    auto mat = mesh->getMaterial();
    auto &sq = mainQueue.at(mat->getShader());
    auto &mq = sq.materials.at(mat);

    mesh->Register(mq.mesh_collection.registerPtr(mesh));
    need_rebuild = true;
}

void SceneRender::unregisterMesh(ptr<Graphics::RenderingMesh> mesh) {
    auto mat = mesh->getMaterial();
    auto &sq = mainQueue.at(mat->getShader());
    auto &mq = sq.materials.at(mat);
    mq.mesh_collection.unregister(mesh->Unregister());
    need_rebuild = true;
}

void SceneRender::setGameDraws(vec<GameDraw> draws) {
    game_draws = std::move(draws);

    // Secondary command buffers, because the frame's main pass is begun with
    // VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: draws cannot be recorded
    // inline into it, only executed sub-passes can.
    if (!game_opaque_cmd) {
        auto mgr = getGraphicsManager();
        game_opaque_cmd = new_ptr<Graphics::CommandBuffer>(mgr, true);
        game_transparent_cmd = new_ptr<Graphics::CommandBuffer>(mgr, true);
    }
    need_rebuild = true;
}

void SceneRender::setPreFrameRecorder(std::function<void(ptr<Graphics::CommandBuffer>)> record) {
    pre_frame = std::move(record);
    need_rebuild = true;
}

void SceneRender::captureFrame(ptr<Graphics::Buffer> dst) {
    capture_dst = std::move(dst);
    need_rebuild = true;
}

/*
 * One secondary command buffer per group (opaque, blended). Each draw binds the
 * pipeline, the per-draw ModelData (binding 0), the per-material block (binding
 * 1) and - when the shader declared them - the scene light block and shadow map,
 * then hands the rest to the game's recorder: vertex buffer, index buffer and
 * `drawIndexedIndirect`. This is the same binding order RenderingMesh::
 * addCommands produces for the CPU chunks, so the shaders cannot tell the two
 * apart (docs/tetrahedral-mesh.md §6.6: "the same material pipeline").
 */
void SceneRender::updateGameCmd() {
    if (!game_opaque_cmd) {
        auto mgr = getGraphicsManager();
        game_opaque_cmd = new_ptr<Graphics::CommandBuffer>(mgr, true);
        game_transparent_cmd = new_ptr<Graphics::CommandBuffer>(mgr, true);
    }

    for (int pass = 0; pass < 2; ++pass) {
        const bool transparent = pass == 1;
        auto cmd = transparent ? game_transparent_cmd : game_opaque_cmd;
        cmd->clear();

        bool any = false;
        for (auto &draw : game_draws) {
            if (draw.transparent != transparent || !draw.material || !draw.record)
                continue;
            auto shader = draw.material->getShader();
            if (!shader)
                continue;

            cmd->setActiveShader(shader);
            if (draw.mesh_data)
                cmd->bindUniformBuffer(draw.mesh_data, Graphics::LightingBindings::kMeshData);
            if (auto ub = draw.material->getUniformBuffer())
                cmd->bindUniformBuffer(ub, 1);
            if (shader->usesLighting() && light_buffer && shadow_map) {
                cmd->bindUniformBuffer(light_buffer, shader->getLightBinding());
                cmd->bindShadowMap(shadow_map, shader->getShadowBinding());
            }
            draw.record(cmd);
            any = true;
        }

        cmd->buildAll();
        (transparent ? game_has_transparent : game_has_opaque) = any;
    }
}

void SceneRender::updateCmd() {
    main_cmd->clear();

    // A compute dispatch cannot be recorded inside a render pass, and the game's
    // mesher has to fill the buffers the draws below read. It goes first, before
    // the shadow pass (which also draws game geometry), and is only recorded on
    // frames SceneRender rebuilds - the game calls markFrameDirty() when it has
    // new cells (docs/tetrahedral-mesh.md §6.4).
    if (pre_frame)
        pre_frame(main_cmd);

    // The depth-only casters pass is recorded BEFORE the window's render pass
    // and outside it: it has its own render pass (a single D32_SFLOAT
    // attachment) and leaves the map in the shader-read layout the scene
    // shaders sample it in.
    //
    // It is recorded whenever the lighting resources exist, also when the scene
    // currently has no light: the pass is what puts the image into that layout,
    // and LightData then holds the neutral (unshadowed) values.
    if (shadow_map && shadow_shader && light_buffer)
        recordShadowPass();

    main_cmd->beginRenderPass({0, 0, 1, 1}, true);
    main_cmd->executeCommandBuffer(clear_cmd);

    // Frame order: every opaque shader queue first, then the blended ones.
    // mainQueue is keyed by shader pointer, so iterating it directly was
    // "whatever order the pointers happen to have": a transparent queue could
    // land before an opaque one and blend against a not yet drawn background.
    // A stable partition on the blend flag keeps the old relative order inside
    // each group, so a scene with no blended shader records exactly as before.
    vec<ShaderQueue *> queues;
    queues.reserve(mainQueue.size());
    for (auto &sq : mainQueue)
        queues.push_back(&sq.second);
    std::stable_sort(queues.begin(), queues.end(),
                     [](const ShaderQueue *a, const ShaderQueue *b) {
                         return int(a->isTransparent()) < int(b->isTransparent());
                     });

    for (ShaderQueue *sq : queues) {
        if (sq->isTransparent())
            continue;
        sq->light_buffer = light_buffer;
        sq->shadow_map = shadow_map;
        sq->updateCmd();
        main_cmd->executeCommandBuffer(sq->cmd);
    }

    // Game-owned opaque draws (the GPU meshed world): after the opaque scene
    // queues, so the depth buffer is complete before the blended pass below.
    updateGameCmd();
    if (game_has_opaque)
        main_cmd->executeCommandBuffer(game_opaque_cmd);

    for (ShaderQueue *sq : queues) {
        if (!sq->isTransparent())
            continue;
        sq->light_buffer = light_buffer;
        sq->shadow_map = shadow_map;
        sq->updateCmd();
        main_cmd->executeCommandBuffer(sq->cmd);
    }

    // Blended game draws (the water pass) go last, after every opaque surface.
    if (game_has_transparent)
        main_cmd->executeCommandBuffer(game_transparent_cmd);

    main_cmd->endRenderPass();

    if (capture_dst) {
        main_cmd->captureColor(capture_dst);
        capture_dst.reset();
    }

    need_rebuild = true;
}

void SceneRender::recordShadowPass() {
    main_cmd->beginShadowPass(shadow_map);
    main_cmd->setActiveShader(shadow_shader);

    // Every registered mesh casts, whatever its material: the pass writes depth
    // only, so it reuses the vertex/index buffers and the per-mesh ModelData of
    // the scene draws (world * light_wvp is computed in shadow.vert).
    // Blended materials are the exception: a transparent surface must not
    // occlude the light, so water-like meshes are skipped here and never
    // appear in the map.
    uint casters = 0;
    uint skipped = 0;
    for (auto &sq : mainQueue)
        for (auto &mq : sq.second.materials) {
            if (mq.second.isTransparent()) {
                ++skipped;
                continue;
            }
            for (auto &mesh : mq.second.mesh_collection.getCollectionWithGaps()) {
                if (!mesh)
                    continue;
                mesh->addShadowCommands(main_cmd, light_buffer,
                                        Graphics::LightingBindings::kLightData);
                ++casters;
            }
        }

    // Game-owned casters: the GPU meshed world is not a RenderingMesh, so it is
    // recorded here with the same depth-only pipeline (the shadow.vert stage reads
    // ModelData at binding 0 and the scene LightData at binding 2, and the mesh
    // vertices carry the same 32-byte layout), and counted so the headless
    // self-test's "the sun renders a shadow map with casters" stays meaningful.
    for (auto &draw : game_draws) {
        if (!draw.cast_shadow || draw.transparent || !draw.record || !draw.material)
            continue;
        auto shader = draw.material->getShader();
        if (!shader || !shader->usesLighting())
            continue;
        if (draw.mesh_data)
            main_cmd->bindUniformBuffer(draw.mesh_data, Graphics::LightingBindings::kMeshData);
        main_cmd->bindUniformBuffer(light_buffer, Graphics::LightingBindings::kLightData);
        draw.record(main_cmd);
        ++casters;
    }

    main_cmd->endShadowPass();
    shadow_map->setCasterCount(casters);

    Log::log(Log::Info, "Shadow pass: {}x{} depth map, {} casters ({} transparent skipped)",
             shadow_map->getSize(), shadow_map->getSize(), casters, skipped);
}

void SceneRender::collectGeometryChanges() {
    for (auto &sq : mainQueue)
        for (auto &mq : sq.second.materials)
            for (auto &mesh : mq.second.mesh_collection.getCollectionWithGaps())
                if (mesh && mesh->consumeCmdDirty())
                    need_rebuild = true;
}

void SceneRender::rebuildCmd() {
    if (need_rebuild) {
        updateCmd();
        main_cmd->buildAll();
        need_rebuild = false;
    }
}

ptr<Components::DirectionalLight> SceneRender::findDirectionalLight() {
    auto sc = scene.lock();
    if (!sc)
        return nullptr;

    ptr<Components::DirectionalLight> found;
    sc->forEachObject([&found](ptr<SceneObjectBase> base) {
        if (found)
            return;
        auto obj = std::dynamic_pointer_cast<Object>(base);
        if (!obj || !obj->isEnabled())
            return;
        for (auto &light : obj->getComponents<Components::DirectionalLight>())
            if (light->isEnabled()) {
                found = light;
                return;
            }
    });
    return found;
}

bool SceneRender::anyShaderUsesLighting() {
    for (auto &sq : mainQueue)
        if (sq.first && sq.first->usesLighting())
            return true;
    return false;
}

void SceneRender::ensureLightingResources(uint map_size) {
    bool created = false;

    if (!light_buffer) {
        light_buffer =
            new_ptr<Graphics::UniformBuffer>(getGraphicsManager(), sizeof(Graphics::LightData));
        light_buffer->updateData(Graphics::LightData{});
        created = true;
    }

    if (!shadow_map || shadow_map->getSize() != map_size) {
        // Sync first: the old map's image and framebuffer may still be
        // referenced by the command buffers recorded for the previous frame.
        getGraphicsManager()->sync();
        shadow_shader.reset();
        shadow_map.reset();

        shadow_map = new_ptr<Graphics::ShadowMap>(getGraphicsManager(), map_size);

        shadow_shader.reset(new Graphics::Shader(getGraphicsManager()));
        shadow_shader->loadShader("shaders/shadow.vert.spv", Graphics::Shader::Vertex);
        // Per-mesh ModelData (binding 0) and the scene LightData (binding 2):
        // the same buffers the scene draws use, so world * light_wvp is computed
        // from live data and the pass never needs a per-frame re-record.
        shadow_shader->addUniformBuffer(Graphics::LightingBindings::kMeshData,
                                        Graphics::Shader::Vertex);
        shadow_shader->addUniformBuffer(Graphics::LightingBindings::kLightData,
                                        Graphics::Shader::Vertex);
        shadow_shader->setVertexStride(sizeof(Graphics::Vertex));
        // Only the position attribute: shadow.vert reads nothing else, and a
        // pipeline that declares attributes its shader does not consume makes
        // the validation layers warn on every pipeline creation.
        shadow_shader->setVertexLayout({Graphics::Vertex::vertexLayout.front()});
        shadow_shader->buildShadowPipeline(map_size);
        created = true;
    }

    if (created)
        need_rebuild = true;
}

void SceneRender::draw(ptr<Components::Camera> camera) {

    auto g_mgr = getEngine()->getGraphicsManager();
    g_mgr->sync();

    // Scene lighting: the first enabled DirectionalLight decides the sun, the
    // shadow parameters and (via its map size) the size of the shadow map. With
    // no light AND no lighting shader nothing below allocates or records
    // anything - the render path stays exactly what it was.
    active_light = findDirectionalLight();
    if (active_light)
        ensureLightingResources(active_light->getMapSize());
    else if (anyShaderUsesLighting()) {
        // A shader asked for the lighting bindings but the scene has no sun:
        // still create the resources so the descriptor sets are complete, and
        // feed them neutral values (see update()) so the surface draws unlit and
        // unshadowed instead of failing descriptor validation.
        if (!warned_missing_light) {
            warned_missing_light = true;
            Log::log(Log::Warning,
                     "A shader declares the lighting bindings but the scene has no enabled "
                     "DirectionalLight; drawing unlit.");
        }
        ensureLightingResources(256);
    }

    // Command buffers are re-recorded only when the queue actually changed
    // (meshes/materials registered, or a mesh got new geometry of a different
    // size); per-frame rebuilds re-recorded every draw command and re-allocated
    // its descriptor set.
    collectGeometryChanges();
    rebuildCmd();
    uint count = update(camera);
    g_mgr->executeCmd(main_cmd);
    // TODO add transparent queue
}

uint SceneRender::update(ptr<Components::Camera> camera) {
    uint count = 0;
    if (!camera)
        return count;

    // Per-frame, per-scene constants. Both used to be recomputed inside the
    // per-mesh loop: getProjectionMatrix() queries SDL_GetWindowSize + tan(),
    // getViewMatrix() inverts the 4x4 transformation matrix - so a scene with N
    // meshes paid for 2N window queries and N matrix inversions per frame.
    // Computing them once here yields the same values (the window cannot resize
    // in the middle of a frame) and keeps resize behaviour unchanged: the next
    // frame queries the new size again.
    const Matrix4x4 view = camera->getViewMatrix();
    const Matrix4x4 projection = camera->getProjectionMatrix();

    // The light frustum is fitted around the camera, so it is rebuilt every
    // frame (a moving camera must not drag its shadow box along). Only the
    // uniform buffer contents change; the recorded draws read it live.
    if (light_buffer) {
        Graphics::LightData data;
        if (active_light) {
            auto cam_mat = camera->getTransform()->getTransformationMatrix();
            const Vector3 center{cam_mat.c14, cam_mat.c24, cam_mat.c34};

            data.light_wvp = Graphics::ShadowMap::buildLightViewProj(
                active_light->getDirection(), center, active_light->getShadowDistance());
            data.ambient = active_light->getAmbient();
            data.diffuse = active_light->getDiffuse();

            const Vector3 dir = active_light->getDirection();
            data.light_dir = {(float)dir.x, (float)dir.y, (float)dir.z, 0.f};
        } else {
            // Neutral light: ambient == diffuse, so lerp(diffuse, ambient, s)
            // is white whatever the shadow factor is, and the old hard-coded
            // light direction keeps the shading identical to the unlit shader.
            data.ambient = {1.f, 1.f, 1.f, 1.f};
            data.diffuse = {1.f, 1.f, 1.f, 1.f};
            Vector3 dir{2, -4, 3};
            dir.normalize();
            data.light_dir = {(float)dir.x, (float)dir.y, (float)dir.z, 0.f};
        }
        light_buffer->updateData(data);
    }

    for (auto &sq : mainQueue)
        for (auto &mq : sq.second.materials) {
            auto &coll = mq.second.mesh_collection.getCollectionWithGaps();
            for (auto &ptr : coll) {
                if (ptr) {
                    ptr->updateConstBuffer(view, projection);
                    ptr->updateCmdBuffer();
                    count++;
                }
            }
        }
    return count;
}

void SceneRender::cleanup() {
    mainQueue.clear();
    shaders.clear();
    materials.clear();
    shadow_shader.reset();
    shadow_map.reset();
    light_buffer.reset();
    // Game-owned state: the capture target and the draw records are buffers the
    // game owns, and cleanup() runs while the device is still alive
    // (ClientScene::close), which is the last chance to release them.
    game_draws.clear();
    game_opaque_cmd.reset();
    game_transparent_cmd.reset();
    pre_frame = nullptr;
    capture_dst.reset();
}

ptr<Graphics::Shader> SceneRender::getShader(String name) {
    try {
        return shaders.at(name);
    } catch (std::out_of_range) {
        THROW_EXCEPTION(fmt::format("Shader {} not found!", name).c_str());
    }
}

ptr<Graphics::Material> SceneRender::getMaterial(String name) {
    try {
        return materials.at(name);
    } catch (std::out_of_range) {
        THROW_EXCEPTION(fmt::format("Material {} not found!", name).c_str());
    }
}

ptr<Graphics::Shader>
SceneRender::getOrCreateShader(String name,
                               std::function<void(ptr<Graphics::Shader>)> shader_creator) {
    auto &sh = shaders[name];
    if (!sh) {
        sh.reset(new Graphics::Shader(getGraphicsManager())); // create shader
        shader_creator(sh); // call creator function to setup shader

        mainQueue.insert({sh, ShaderQueue(sh, getEngine()->getGraphicsManager())});
        need_rebuild = true;
    }
    return sh;
}

ptr<Graphics::Material> SceneRender::getOrCreateMaterial(
    String name, std::function<ptr<Graphics::Material>(ptr<SceneRender>)> material_factory) {
    auto &m = materials[name];
    if (!m) {
        m = material_factory(shared_from_this());
        auto &shader_q = mainQueue.at(m->getShader());
        shader_q.materials.insert({m, MaterialQueue(m)});
        need_rebuild = true;
    }
    return m;
}

ptr<ClientEngine> SceneRender::getEngine() {
    auto engine_lock = engine.lock();
    if (!engine_lock) {
        THROW_NULL_PTR_EXCEPTION(engine_lock.get());
    }
    return engine_lock;
}

ptr<Graphics::GraphicsManager> SceneRender::getGraphicsManager() {
    return getEngine()->getGraphicsManager();
}

NSP_ENGINE_END

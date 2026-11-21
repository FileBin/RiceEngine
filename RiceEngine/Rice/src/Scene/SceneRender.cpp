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

    for (auto &mq : materials) {
        mq.second.addCommands(cmd);
    }
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

void SceneRender::updateCmd() {
    main_cmd->clear();

    // The depth-only casters pass is recorded BEFORE the window's render pass
    // and outside it: it has its own render pass (a single D32_SFLOAT
    // attachment) and leaves the map in the shader-read layout the scene
    // shaders sample it in.
    if (shadow_map && shadow_shader && light_buffer)
        recordShadowPass();

    main_cmd->beginRenderPass({0, 0, 1, 1}, true);
    main_cmd->executeCommandBuffer(clear_cmd);
    for (auto &sq : mainQueue) {
        sq.second.light_buffer = light_buffer;
        sq.second.shadow_map = shadow_map;
        sq.second.updateCmd();
        main_cmd->executeCommandBuffer(sq.second.cmd);
    }

    main_cmd->endRenderPass();

    need_rebuild = true;
}

void SceneRender::recordShadowPass() {
    main_cmd->beginShadowPass(shadow_map);
    main_cmd->setActiveShader(shadow_shader);

    // Every registered mesh casts, whatever its material: the pass writes depth
    // only, so it reuses the vertex/index buffers and the per-mesh ModelData of
    // the scene draws (world * light_wvp is computed in shadow.vert).
    uint casters = 0;
    for (auto &sq : mainQueue)
        for (auto &mq : sq.second.materials) {
            for (auto &mesh : mq.second.mesh_collection.getCollectionWithGaps()) {
                if (!mesh)
                    continue;
                mesh->addShadowCommands(main_cmd, light_buffer,
                                        Graphics::LightingBindings::kLightData);
                ++casters;
            }
        }

    main_cmd->endShadowPass();
    shadow_map->setCasterCount(casters);

    Log::log(Log::Info, "Shadow pass: {}x{} depth map, {} casters", shadow_map->getSize(),
             shadow_map->getSize(), casters);
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

#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/LightData.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Shader.hpp"
#include "Rice/GL/ShadowMap.hpp"
#include "Rice/stdafx.hpp"

NSP_ENGINE_BEGIN
class SceneRender;
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN
class Camera;
class DirectionalLight;
NSP_COMPONENTS_END

#pragma once

#include "ClientScene.hpp"
#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/GL/RenderingMesh.hpp"

NSP_GL_BEGIN
class RenderingMesh;
NSP_GL_END

NSP_ENGINE_BEGIN

class SceneRender : public enable_ptr<SceneRender> {
  private:
    struct MaterialQueue {
        ptr<Graphics::Material> material;
        Util::RegisterCollection<Graphics::RenderingMesh, size_t> mesh_collection;

        MaterialQueue(ptr<Graphics::Material> mat);

        void addCommands(ptr<Graphics::CommandBuffer> cmd);
    };

    struct ShaderQueue {
        ptr<Graphics::CommandBuffer> cmd;
        ptr<Graphics::Shader> shader;
        map<ptr<Graphics::Material>, MaterialQueue> materials;

        /* Filled in by SceneRender::updateCmd for shaders built with
         * Shader::addLighting(): the scene light block and the shadow map that
         * every draw of this queue binds. Null when the scene has no lighting
         * resources; then nothing extra is bound and the shader keeps the plain
         * bindings 0/1 layout it was built with. */
        ptr<Graphics::UniformBuffer> light_buffer;
        ptr<Graphics::ShadowMap> shadow_map;

        ShaderQueue(ptr<Graphics::Shader> shader, ptr<Graphics::GraphicsManager> mgr);
        void updateCmd();
    };

    map<String, ptr<Graphics::Shader>> shaders;
    map<String, ptr<Graphics::Material>> materials;

    wptr<ClientScene> scene;
    wptr<ClientEngine> engine;
    ptr<Graphics::CommandBuffer> main_cmd, clear_cmd;

    map<ptr<Graphics::Shader>, ShaderQueue> mainQueue;

    /* Scene lighting: the first enabled DirectionalLight is looked up every
     * frame, and the shadow map / depth-only pipeline / LightData buffer are
     * created on the first frame that needs them. All of them stay null in a
     * scene without a light and without a lighting shader, which is what keeps
     * the unlit demos on the exact old render path (no shadow pass, no extra
     * bindings, nothing allocated). */
    ptr<Components::DirectionalLight> active_light;
    ptr<Graphics::ShadowMap> shadow_map;
    ptr<Graphics::Shader> shadow_shader;
    ptr<Graphics::UniformBuffer> light_buffer;
    bool warned_missing_light = false;

    bool need_rebuild = true;

    void updateCmd();
    void rebuildCmd();
    // Consumes RenderingMesh::cmd_dirty flags: a mesh whose geometry was
    // replaced (resetMesh) needs the DrawIndexed count recorded again.
    void collectGeometryChanges();

    // First enabled DirectionalLight of the scene, or nullptr.
    ptr<Components::DirectionalLight> findDirectionalLight();
    // True when at least one registered shader declared the lighting bindings.
    bool anyShaderUsesLighting();
    // Creates the shadow map (`map_size` square), the depth-only casters
    // pipeline and the LightData buffer if they do not exist yet, and forces a
    // re-record when it does. Safe to call every frame; a no-op once set up.
    void ensureLightingResources(uint map_size);
    // Records the depth-only casters pass at the head of main_cmd.
    void recordShadowPass();

    uint update(ptr<Components::Camera> camera);

  public:
    SceneRender(ptr<ClientScene> scene, ptr<ClientEngine> engine);
    void registerMesh(ptr<Graphics::RenderingMesh> mesh);
    void unregisterMesh(ptr<Graphics::RenderingMesh> mesh);
    void draw(ptr<Components::Camera> camera);
    void cleanup();

    ptr<Graphics::Shader> getShader(String name);

    ptr<Graphics::Material> getMaterial(String name);

    ptr<Graphics::Shader>
    getOrCreateShader(String name, std::function<void(ptr<Graphics::Shader>)> shader_creator);
    ptr<Graphics::Material> getOrCreateMaterial(
        String name,
        std::function<ptr<Graphics::Material>(ptr<SceneRender>)> material_factory);

    ptr<Graphics::GraphicsManager> getGraphicsManager();

    /* The scene's shadow map, or null when nothing has asked for lighting yet.
     * getCasterCount() on it is the number of meshes the last recorded casters
     * pass drew. */
    ptr<Graphics::ShadowMap> getShadowMap() { return shadow_map; }

    /* The DirectionalLight the last drawn frame used, or null. */
    ptr<Components::DirectionalLight> getActiveLight() { return active_light; }

    ptr<ClientEngine> getEngine();
    ptr<ClientScene> getScene();
};

NSP_ENGINE_END

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

#include <functional>

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

        // Blended material: drawn after the opaque ones of the same shader and
        // never recorded into the shadow casters pass.
        bool isTransparent() const { return material->isTransparent(); }
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

        // The pipeline this queue draws with is alpha blended: the whole queue
        // goes after the opaque ones in the frame.
        bool isTransparent() const { return shader->isBlendEnabled(); }
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

    /* Game-owned draws (§M6 of docs/tetrahedral-mesh.md in the game repo): the
     * GPU mesher's vertex/index buffers and indirect records are not a
     * RenderingMesh, so the scene records them here, inside its own render pass
     * and with its own material/lighting bindings. The state is at the end of the
     * class, after GameDraw is defined. */

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

    /* ---------------------------------------------------------------------
     * Game-owned geometry (docs/tetrahedral-mesh.md §6.1's injection point,
     * §6.3's buffers). A draw whose vertex/index/indirect buffers the game owns
     * - the GPU mesher's pages - is not a `Graphics::RenderingMesh`: there is no
     * CPU-side Mesh to upload and no per-mesh index count to bake. Instead the
     * game hands the scene:
     *
     *   material  the pipeline (its shader) and the per-material block at
     *             binding 1 - the same materials the CPU chunks used;
     *   mesh_data the per-draw ModelData at binding 0 (the camera matrices, the
     *             same block a RenderingMesh keeps);
     *   record    a callback that binds the game's buffers and issues the
     *             draw(s), e.g. `cmd->drawIndexedIndirect(page.draws, range, 1)`.
     *
     * The scene records them in its frame: inside the main render pass, after
     * the opaque scene queues (`transparent == false`) or after the blended ones
     * (`transparent == true`), with the scene light block and shadow map bound
     * when the material's shader declares them. `cast_shadow` also records the
     * draw into the depth-only casters pass with the engine's shadow pipeline.
     *
     * setGameDraws() forces one frame re-record; call it only when a buffer
     * handle changed, not per frame (the recorded draws read the buffers live,
     * so new contents do not need a re-record).
     * ------------------------------------------------------------------- */
    struct GameDraw {
        ptr<Graphics::Material> material;
        ptr<Graphics::UniformBuffer> mesh_data;
        std::function<void(ptr<Graphics::CommandBuffer>)> record;
        bool transparent = false;
        bool cast_shadow = false;
    };
    void setGameDraws(vec<GameDraw> draws);

    /* Compute work that has to run before the frame's render passes: a dispatch
     * cannot be recorded inside one, and the GPU mesher has to fill the vertex,
     * index and indirect buffers before the draws that read them. Called at the
     * top of the frame's command buffer, before the shadow pass, and only on the
     * frames that are re-recorded - call markFrameDirty() on a frame that has
     * new cells to dispatch. Record dispatch()/memoryBarrier() here. */
    void setPreFrameRecorder(std::function<void(ptr<Graphics::CommandBuffer>)> record);

    /* Copies the next recorded frame's colour image into `dst` (a StorageBuffer
     * of at least width * height * 4 bytes) after the main pass: the pixel
     * coverage check of the headless self-test. The bytes are readable after the
     * following GraphicsManager::sync(). One-shot: cleared once recorded. */
    void captureFrame(ptr<Graphics::Buffer> dst);

    /* Re-records the next frame (pre-frame dispatches, game draws, capture).
     * Cheap to over-call - it only sets the flag updateCmd() already consumes. */
    void markFrameDirty() { need_rebuild = true; }

  private:
    // Game-owned draw state; declared after GameDraw for the vector's element type.
    vec<GameDraw> game_draws;
    ptr<Graphics::CommandBuffer> game_opaque_cmd, game_transparent_cmd;
    bool game_has_opaque = false, game_has_transparent = false;
    std::function<void(ptr<Graphics::CommandBuffer>)> pre_frame;
    ptr<Graphics::Buffer> capture_dst;
    // Records the two game sub-passes from game_draws and the lighting resources.
    void updateGameCmd();
};

NSP_ENGINE_END

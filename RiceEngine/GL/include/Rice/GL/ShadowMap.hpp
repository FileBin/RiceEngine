/*
 * ShadowMap.hpp — the depth-only shadow map of the scene's directional light.
 *
 * A ShadowMap owns:
 *   - a size x size D32_SFLOAT depth image + view, renderable as a depth
 *     attachment AND sampleable as a plain (non-comparison) texture;
 *   - the sampler used to sample it: nearest / clamp-to-edge, compare OFF (the
 *     shader loads raw depth with texelFetch and compares itself, exactly like
 *     the original DirectX implementation did);
 *   - a framebuffer for it, bound to the engine's depth-only shadow render pass
 *     (GraphicsManager_API_data::shadow_renderPass).
 *
 * It does NOT own the light matrices or the pipeline: SceneRender computes the
 * light matrices (ShadowMap::buildLightViewProj), uploads them into the scene
 * LightData uniform buffer, and records the casters pass with the shadow shader
 * (Shader::buildShadowPipeline).
 *
 * Bindings: in a lit draw the map is set 0, binding 3 (`LightingBindings::
 * kShadowMap`, `sampler2D shadow_map`); inside the casters pass the shadow shader
 * reads the per-mesh ModelData at binding 0 and the scene LightData at binding 2.
 */

#include "stdafx.hpp"

NSP_GL_BEGIN
class ShadowMap;
struct ShadowMap_API_Data;
NSP_GL_END

#pragma once

#include "Rice/GL/GraphicsComponentBase.hpp"
#include "Rice/Math/Matrixes.hpp"
#include "Rice/Math/Vectors.hpp"

NSP_GL_BEGIN

class ShadowMap : public GraphicsComponentBase {
  public:
    /* Default map resolution. The ported 9x9 PCF (shadow.glsl) spreads its taps
     * 0.5 texels apart, so this is also the value the softness was tuned for. */
    static constexpr uint kDefaultSize = 2048;

    /*
     * `size` is clamped to [256, 8192] and rounded down to a power of two. The
     * image is allocated immediately; the shader path is
     * "shaders/shadow.vert.spv" relative to the working directory (the casters
     * pass vertex shader).
     */
    ShadowMap(ptr<GraphicsManager> g_mgr, uint size = kDefaultSize);
    ~ShadowMap() override;

    void cleanup() override;

    uint getSize() const;

    /* How many meshes the last recorded casters pass drew. 0 means the map was
     * allocated but never filled — the smoke check treats that as a failure. */
    uint getCasterCount() const;
    void setCasterCount(uint count);

    /**
     * Debug/verification helper: waits for the GPU, copies the whole image back
     * to host memory and returns the number of texels that hold geometry (depth
     * < 1.0). A map that was "rendered" but is uniformly far/empty returns 0.
     *
     * It runs its own one-time command buffer and stalls the pipeline, so it is a
     * test/introspection call, not something to do per frame.
     */
    uint readFilledTexelCount();

    /**
     * The light's view * projection matrix for one frame, row-vector convention
     * (a vertex is transformed as `pos * world * light_wvp`).
     *
     * Both matrices target the engine's depth range: the orthographic projection
     * maps z to [0, 1] (Vulkan clip space), so the value the fragment shader gets
     * in `lPos.z` is directly comparable with the depth stored in the map.
     *
     *   - `light_direction` is the direction the rays travel (world space); it is
     *     normalised here, and a zero vector is replaced by "straight down";
     *   - `center` is the point the frustum is centred on — the active camera;
     *   - `shadow_distance` is the HALF width of the square the projection
     *     covers, so the map spans 2 * shadow_distance world units across. The
     *     near plane is the light's eye position (center - direction * distance)
     *     and the far plane is 2 * distance away from it.
     *
     * Because the frustum follows the camera, a mesh more than `shadow_distance`
     * away is outside the map; the shader fades those out to "fully lit" instead
     * of shadowing them (the border fade in shadow.glsl).
     */
    static Matrix4x4 buildLightViewProj(Vector3 light_direction, Vector3 center,
                                        dbl shadow_distance);

  private:
    friend struct CommandBuffer_API_data;
    friend struct DescriptorSetCreator;

    uptr<ShadowMap_API_Data> api_data;
};

NSP_GL_END

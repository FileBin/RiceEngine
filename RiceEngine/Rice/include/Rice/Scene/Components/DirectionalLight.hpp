#include "../../stdafx.hpp"

NSP_COMPONENTS_BEGIN
class DirectionalLight;
NSP_COMPONENTS_END

#pragma once

#include "Rice/Math/Vectors.hpp"
#include "Rice/Scene/Component.hpp"

NSP_COMPONENTS_BEGIN

/**
 * DirectionalLight — a sun: one direction, one ambient term, one diffuse term,
 * and the parameters of the shadow map that is rendered for it.
 *
 * A SceneRender looks for the FIRST enabled DirectionalLight of the scene once
 * per frame (SceneRender::draw) and, when it finds one, renders a depth-only
 * shadow pass and hands `ambient` / `diffuse` / `direction` + the light matrices
 * to every material whose shader was built with Shader::addLighting(). With no
 * enabled DirectionalLight in the scene nothing changes at all: no shadow map
 * is allocated, no shadow pass is recorded, the scene shaders draw exactly as
 * before.
 *
 *     auto sun_obj = createEnabled("Sun");
 *     auto sun = new_ptr<Components::DirectionalLight>();
 *     sun->setDirection({-0.45, -1, -0.35});   // rays travel down and +z
 *     sun->setAmbient({0.25, 0.25, 0.3, 1});
 *     sun->setDiffuse({1, 1, 1, 1});
 *     sun_obj->addComponent(sun);
 *
 * The component only carries data; it does not need a Transform (the shadow
 * frustum is fitted around the active camera).
 */
class DirectionalLight : public virtual Components::Component {
  private:
    Vector3 direction = {-0.45, -1, -0.35};
    Vector4f ambient = {0.25f, 0.25f, 0.3f, 1.f};
    Vector4f diffuse = {1.f, 1.f, 1.f, 1.f};
    dbl shadow_distance = 256;
    uint map_size = 2048;

  public:
    /**
     * Direction the light rays travel, in world space (so a sun above the
     * scene is, e.g., {0, -1, 0}). Stored normalised; a zero-length vector is
     * ignored and the previous direction kept, because a zero direction cannot
     * build a light view matrix.
     */
    void setDirection(Vector3 dir) {
        dbl len = dir.length();
        if (len > 1e-9)
            direction = dir / len;
    }

    /** Unit vector: the direction the light travels. */
    Vector3 getDirection() const { return direction; }

    /** Ambient colour, rgb used (w ignored). This is the colour of a fully
     * shadowed / unlit surface: the scene term lerps diffuse -> ambient by the
     * shadow factor. */
    void setAmbient(Vector4f color) { ambient = color; }
    Vector4f getAmbient() const { return ambient; }

    /** Diffuse colour of a fully lit surface, rgb used (w ignored). */
    void setDiffuse(Vector4f color) { diffuse = color; }
    Vector4f getDiffuse() const { return diffuse; }

    /**
     * Half the width of the square the light's orthographic projection covers,
     * centred on the active camera. The shadow map therefore spans
     * 2 * shadow_distance world units across; geometry outside it gets the
     * border fade from the shader (fully lit). Default 256 (a 512 unit square).
     */
    void setShadowDistance(dbl dist) { shadow_distance = std::clamp(dist, 1.0, 65536.0); }
    dbl getShadowDistance() const { return shadow_distance; }

    /**
     * Resolution of this light's shadow map, square. Clamped to a sane range
     * and rounded down to a power of two (the map is rebuilt when the value
     * changes). Default 2048 — the value the ported 9x9 PCF was tuned for.
     */
    void setMapSize(uint size) {
        if (size < 64)
            size = 64;
        if (size > 8192)
            size = 8192;
        uint pow2 = 64;
        while (pow2 * 2 <= size)
            pow2 *= 2;
        map_size = pow2;
    }
    uint getMapSize() const { return map_size; }
};

NSP_COMPONENTS_END

/*
 * LightData.hpp — the scene lighting block that every lit draw reads, and the
 * descriptor-set bindings it lives at.
 *
 * The engine has ONE descriptor set per draw (set 0), whose layout is built from
 * the Shader::addUniformBuffer / Shader::addLighting calls of the shader. The
 * numbers below are that set's binding numbers; they are fixed because the scene
 * shaders in Tests/shaders/scene hard-code them:
 *
 *   binding 0  uniform ModelData     (per mesh, RenderingMesh::constBuffer)   vertex
 *   binding 1  material uniforms     (per material, Material::uniformBuffer)  fragment
 *   binding 2  uniform LightData     (per scene, SceneRender)           vertex+fragment
 *   binding 3  sampler2D shadow_map  (per scene, ShadowMap)             fragment
 *
 * Only shaders built with Shader::addLighting() declare 2 and 3; every other
 * shader keeps the old layout of bindings 0 and 1 and is drawn exactly as before.
 */

#include "stdafx.hpp"

NSP_GL_BEGIN
struct LightData;
struct LightingBindings;
NSP_GL_END

#pragma once

#include "Rice/Math/Matrixes.hpp"
#include "Rice/Math/Vectors.hpp"

NSP_GL_BEGIN

/* Binding numbers of the scene-wide lighting resources inside set 0. */
struct LightingBindings {
    static constexpr uint kMeshData = 0;
    static constexpr uint kMaterial = 1;
    static constexpr uint kLightData = 2;
    static constexpr uint kShadowMap = 3;
};

/**
 * std140 layout of the scene lighting block, uniform buffer at binding 2:
 *
 *   layout(std140, binding = 2) uniform LightBlock {
 *       mat4 light_wvp;   // world -> light clip space. Multiplied by the world
 *                         // matrix in the shader; y is flipped at the end, the
 *                         // same way the camera projection is.
 *       vec3 ambient;     // colour of a fully shadowed surface
 *       vec3 diffuse;     // colour of a fully lit surface
 *       vec3 light_dir;   // direction the light rays travel, world space
 *   } light;
 *
 * In GLSL a std140 `vec3` still occupies a 16-byte slot, so the three colours are
 * stored as Vector4f here (their .xyz is what std140 exposes as the vec3): the
 * byte offsets are 0, 64, 80, 96 and sizeof(LightData) is 112.
 */
struct alignas(16) LightData {
    Matrix4x4f light_wvp = Matrix4x4f::identity;
    Vector4f ambient = {0.f, 0.f, 0.f, 0.f};
    Vector4f diffuse = {1.f, 1.f, 1.f, 1.f};
    Vector4f light_dir = {0.f, -1.f, 0.f, 0.f};
};

static_assert(sizeof(LightData) == 112, "std140: mat4 + three padded vec3 slots");
static_assert(offsetof(LightData, ambient) == 64, "std140: vec3 after mat4");
static_assert(offsetof(LightData, diffuse) == 80, "std140: vec3 slots are 16-byte aligned");
static_assert(offsetof(LightData, light_dir) == 96, "std140: vec3 slots are 16-byte aligned");

NSP_GL_END

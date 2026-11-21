/*
 * shadow.glsl — shadow map sampling and the directional light term.
 *
 * Port of the original DirectX implementation
 * (GameMain/shaders/Shadows.hlsli + LightBuffer.hlsli):
 *
 *   SampleShadowMap()  2x2 interpolated raw depth fetch
 *   GetLight()         9x9 PCF, bias = 0.005 - lightSpaceZ, border fade,
 *                      lerp(diffuse, ambient, shadow)
 *
 * The maths is kept identical on purpose:
 *
 *   - the sampler has no comparison state (sampler compare OFF), the depth is
 *     loaded with texelFetch and compared here, in the shader, like Texture2D
 *     .Load() did;
 *   - the shadow factor is `saturate((lightSpaceZ - mapDepth - bias) * 100)`;
 *   - the last term is `mix(diffuse.xyz, ambient.xyz, shadow)`, i.e. it returns
 *     the LIGHT COLOUR, which the caller multiplies into its own shading;
 *   - a shadow lookup outside the map fades to "fully lit" instead of black.
 *
 * Differences from the HLSL are mechanical only: SHADOWMAP_SIZE (a compile-time
 * define) became textureSize(), and the texel clamp that Texture2D.Load()
 * performs implicitly is explicit.
 */

#include "light.glsl"

/* Set 0, binding 3 (Rice::Graphics::LightingBindings::kShadowMap). */
layout(binding = 3) uniform sampler2D shadow_map;

vec2 ShadowMapSize() { return vec2(textureSize(shadow_map, 0)); }

float LoadShadowMap(ivec2 pos) {
    // Texture2D.Load() clamps out-of-range fetches at the edge; texelFetch is
    // undefined there, so clamp explicitly.
    ivec2 maxPos = textureSize(shadow_map, 0) - ivec2(1, 1);
    return texelFetch(shadow_map, clamp(pos, ivec2(0, 0), maxPos), 0).x;
}

/*
 * One bilinear-style fetch built from four raw texel loads. `bias` is added to
 * the stored depths before the saturate, exactly like the HLSL: samples are
 * "lit" (1) where the receiver is in front of the stored depth.
 */
float SampleShadowMap(vec2 pixelCoords, float bias) {
    ivec2 fpos = ivec2(floor(pixelCoords));

    float s00 = LoadShadowMap(fpos + ivec2(0, 0)) + bias;
    float s10 = LoadShadowMap(fpos + ivec2(1, 0)) + bias;
    float s01 = LoadShadowMap(fpos + ivec2(0, 1)) + bias;
    float s11 = LoadShadowMap(fpos + ivec2(1, 1)) + bias;

    // saturate((-samples) * 100): stored depth + bias >= 0 lights the sample.
    vec4 samples = clamp(-vec4(s00, s10, s01, s11) * 100.0, 0.0, 1.0);

    vec2 t = pixelCoords - vec2(fpos);
    t = vec2(.5) - cos(t * 3.14159265359) * .5;

    return (samples.x * (1.0 - t.x) + samples.y * t.x) * (1.0 - t.y) +
           (samples.z * (1.0 - t.x) + samples.w * t.x) * t.y;
}

/*
 * lPos is the interpolated light-space position of the fragment
 * (world_pos * light.light_wvp, see default.vert). Returns the light colour for
 * this fragment: diffuse where lit, ambient where shadowed.
 */
vec3 GetLight(vec4 lPos) {
    float shadow = 0.0;

    lPos.xyz /= lPos.w;

    // Border fade: near the edge of the map there is no reliable depth, so fade
    // to "fully lit" over the outer quarter instead of shadowing everything.
    shadow = 1.0 - clamp(max(abs(lPos.x), abs(lPos.y)) * 4.0 - 3.0, 0.0, 1.0);

    lPos.x = lPos.x / 2.0 + 0.5;
    // The map was rasterized with the y flip (shadow.vert), so the lookup flips
    // again to land on the same texel — the original HLSL convention.
    lPos.y = lPos.y / -2.0 + 0.5;
    vec2 pixelCoords = lPos.xy * ShadowMapSize();

    float fac = 0.0;
    float bias = .005;

    const int nSamples = 4;   // 9x9 taps
    const float e = .5;       // half a texel between taps

    for (int i = -nSamples; i <= nSamples; i++) {
        for (int j = -nSamples; j <= nSamples; j++) {
            fac += SampleShadowMap(pixelCoords + vec2(i, j) * e, bias - lPos.z);
        }
    }
    fac /= 81.0;

    shadow *= fac;

    return mix(light.diffuse, light.ambient, shadow);
}

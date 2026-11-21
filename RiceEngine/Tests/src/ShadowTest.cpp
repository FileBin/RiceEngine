/*
 * ShadowTest — directional light + shadow map smoke test.
 *
 * Scene: a 40x40 ground slab, four cubes on it, one Components::DirectionalLight
 * and a camera looking at the group. The material is built from the LIT scene
 * shaders (default_lit.vert.spv / diffuse_lit.frag.spv, i.e. default.vert and
 * diffuse.frag compiled with -DRICE_LIGHTING) so the draw reads the scene light
 * block at binding 2 and the shadow map at binding 3.
 *
 * Each frame it checks that:
 *
 *   1. SceneRender found the DirectionalLight (getActiveLight() != null);
 *   2. a 2048x2048 shadow map exists for it;
 *   3. the casters pass actually drew geometry into it (getCasterCount() > 0) —
 *      an allocated but empty map is the failure mode this guards against.
 *
 * It closes its own window after 60 frames and exits 0, or 1 if any invariant
 * broke. Run under the validation layers (Debug build) so that a bad image
 * layout, a descriptor set used before it was filled or a pipeline that does not
 * match the shaders shows up as an [ERROR] line.
 */

#include <SDL.h>

#include <cstdio>

#include "Rice/Engine.hpp"
#include "Rice/Engine/DefaultCoreLoader.hpp"
#include "Rice/Tests/Scenes/ShadowTest.hpp"

int main() {
    using namespace Rice;
    using namespace Rice::Tests;

    std::printf("ShadowTest: starting\n");

    ClientCore::runNew(new_ptr<DefaultCoreLoader>("ShadowTest", ShadowTestScene::create()));

    if (ShadowTestStats::failures != 0) {
        std::printf("ShadowTest: %u failure(s)\n", ShadowTestStats::failures);
        return 1;
    }

    std::printf("ShadowTest: done, %u frames, %ux%u shadow map, %u casters, %u filled texels, "
                "light found\n",
                ShadowTestStats::frames, ShadowTestStats::map_size, ShadowTestStats::map_size,
                ShadowTestStats::casters, ShadowTestStats::filled_texels);
    return 0;
}

#pragma once

#include "WorldSeed.h"
#include "WorldGenerator.h"

#include "Voxels\VoxelVoid.h"
#include "Voxels\VoxelGrass.h"
#include <Scripts\Util\PerlinNoise.h>

class StandartGenerator : public WorldGenerator {
private:
public:
    struct WorldParams {
        dbl caveIntensity;
        dbl minHeight;
        dbl maxHeight;
        WorldSeed seed;
    } params;
    StandartGenerator(WorldSeed seed, dbl maxHeight = 120, dbl minHeight = 60, dbl caveIntensity = 0.4) {
        params.caveIntensity = caveIntensity;
        params.maxHeight = maxHeight;
        params.minHeight = minHeight;
        params.seed = seed;
    }
    dbl GetTerrainHeight(Vector2 pos) {
        using namespace Utilites;
        auto n = params.seed.heightMapSeeds.size();
        auto terrainHeight = params.minHeight;
        dbl s = pow(2, n - 1);
        dbl sy = params.maxHeight - params.minHeight;
        for (size_t k = 0; k < n; k++) {
            sy /= 2;
            auto p = pos / s;
            dbl h = PerlinNoise(p, params.seed.heightMapSeeds[k]);
            h *= sy;
            terrainHeight += h;
            s /= 2;
        }
        return terrainHeight;
    }
    Voxel& GetVoxel(Vector3 pos, dbl groundAltitude) {
        auto d = groundAltitude;
        if (groundAltitude > 0) {
            return *(new VoxelVoid(pos));
        }
        Vector3 n;
        auto cd = CaveDepth(pos) * 20.;

        auto v = d - cd;

        //v *= 0.5f;
        v += 2.;

        v = min(max(v, 0), 1);

        d = std::lerp(cd,d,v);

        d = max(cd, d);
        if (cd > 0) {
            return *(new VoxelVoid(pos));
        }
        return *(new VoxelGrass(pos));
    }

private:
    dbl CaveDepth(Vector3 pos) {
        using namespace Utilites;
        dbl caveFactor = 0.;
        auto& seeds = params.seed.CaveSeeds;
        num n = seeds.size();
        dbl s = pow(2., -n);
        dbl h = 1.;
        for (size_t i = 0; i < n; i++) {
            h /= 2;
            s *= 2;
            caveFactor += PerlinNoise(pos * s, seeds[i]) * h;
        }
        return (caveFactor - 1 + params.caveIntensity);
    }
};

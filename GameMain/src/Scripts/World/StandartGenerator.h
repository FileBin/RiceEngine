#pragma once

#include "WorldSeed.h"
#include "WorldGenerator.h"

#include "Voxels\VoxelVoid.h"
#include "Voxels\VoxelGrass.h"
#include "Voxels\VoxelDirt.h"
#include "Voxels\VoxelStone.h"
#include "Voxels\VoxelSnow.h"
#include "Voxels\VoxelDarkStone.h"
#include "Voxels\VoxelDarkGrass.h"
#include "Voxels\VoxelWater.h"
#include <Scripts\Util\PerlinNoise.h>

class StandartGenerator : public WorldGenerator {
private:
public:
    struct WorldParams {
        dbl caveIntensity;
        dbl minHeight;
        dbl maxHeight;
        dbl landsclapeScale;
        dbl caveScale;
        WorldSeed seed;
    } params;
    StandartGenerator(WorldSeed seed, dbl maxHeight = 120, dbl minHeight = 60, dbl caveIntensity = 0.4, dbl landscapeScale = 1, dbl caveScale = 4) {
        params.caveIntensity = caveIntensity;
        params.maxHeight = maxHeight;
        params.minHeight = minHeight;
        params.seed = seed;
        params.landsclapeScale = 1/landscapeScale;
        params.caveScale = 1/caveScale;
    }
    dbl GetTerrainHeight(Vector2 pos) {
        using namespace Utilites;
        auto n = params.seed.heightMapSeeds.size();
        auto terrainHeight = params.minHeight;
        dbl s = pow(2, n - 1);
        dbl sy = params.maxHeight - params.minHeight;
        for (size_t k = 0; k < n; k++) {
            sy /= 2;
            auto p = pos / s * params.landsclapeScale;
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
            if (pos.y == -7) {
                return *(new VoxelWater(pos)); // lakes
            }
            else {
                return *(new VoxelVoid(pos)); // sky
            }
        }
        Vector3 n;
        auto cd = CaveDepth(pos) * 20.;

        if (d < -1.7 && cd < 1 && pos.y > -10) {
            return *(new VoxelDirt(pos)); // partially underground layer
        }
        else if (d < -1.7 && cd < 1) {
            return *(new VoxelDarkStone(pos)); // fully underground
        }

        auto v = d - cd;

        //v *= 0.5f;
        v += 2.;

        v = min(max(v, 0), 1);

        d = std::lerp(cd,d,v);

        d = max(cd, d);

        if (cd > -5) {
            if (pos.y > 30 - cd * 2) {
                return *(new VoxelSnow(pos)); // mountain tops
            }
            else if (pos.y < 20 && cd > 0) {
                return *(new VoxelVoid(pos)); // caves
            }
            else {
                return *(new VoxelStone(pos)); // middle mountains
            }
        }
        else {
            if (pos.y < 20 - cd / 2.0) {
                if (pos.y < - cd / 1.7) {
                    return *(new VoxelDarkGrass(pos)); // grass bottom layer
                }
                else {
                    return *(new VoxelGrass(pos)); // grass top layer
                }
            }
            else {
                return *(new VoxelStone(pos)); // upper mountains
            }
        }

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
            s *= 1.7;
            caveFactor += PerlinNoise(pos * s * params.caveScale, seeds[i]) * h;
        }
        return (caveFactor - 1 + params.caveIntensity);
    }
};

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

    StandartGenerator(WorldSeed seed, dbl maxHeight = 120, dbl minHeight = 60, dbl caveIntensity = 0.4, dbl landscapeScale = 1, dbl caveScale = 1) {
        params.caveIntensity = caveIntensity;
        params.maxHeight = maxHeight;
        params.minHeight = minHeight;
        params.seed = seed;
        params.landsclapeScale = 1/landscapeScale;
        params.caveScale = 1/caveScale;
    }

    const dbl sMultipiler = 2;
    const float waterLevel = -8;

    dbl GetTerrainHeight(Vector2 pos) {

        using namespace Utilites;
        auto n = params.seed.heightMapSeeds.size();
        auto terrainHeight = params.minHeight;
        dbl s = pow(sMultipiler, n - 1);
        dbl sy = params.maxHeight - params.minHeight;
        for (size_t k = 0; k < n; k++) {
            sy /= 2;
            auto p = pos / s * params.landsclapeScale;
            dbl h = PerlinNoise(p, params.seed.heightMapSeeds[k]);
            h *= sy;
            terrainHeight += h;
            s /=sMultipiler;
        }
        return terrainHeight;
    }

    VoxelData GetVoxelData(Vector3 pos, dbl groundAltitude) {
        float d = (float)groundAltitude;
        if (groundAltitude > 0) {
            if (pos.y == waterLevel) {
                return { VoxelTypeIndex::V_WATER, d + waterLevel}; // lakes
            }
            else {
                return { VoxelTypeIndex::V_VOID, d }; // sky
            }
        }

        //Vector3 n;

        float cd = CaveDepth(pos) * 20.;

        float v = d - cd;

        v *= 0.5f;
        v += 2.;

        v = Math::Clamp01(v);

        d = Math::Lerp(cd, d, v); //smoothing between caves and terrain

        d = max(cd, d);

        if (d < -1.7 && cd < 2 && pos.y > -10) {
            return { VoxelTypeIndex::V_DIRT, d }; // partially underground layer
        }
        else if (d < -1.7 && cd < 2) {
            return { VoxelTypeIndex::V_DARK_STONE, d }; // fully underground
        }

        if (cd > -5) {
            if (pos.y > 30 - cd * 2) {
                return { VoxelTypeIndex::V_SNOW, d }; // mountain tops
            }
            else if (pos.y < 20 && cd > 0) {
                return { VoxelTypeIndex::V_VOID, d }; // caves
            }
            else {
                return { VoxelTypeIndex::V_STONE, d }; // middle mountains
            }
        }
        else {
            if (pos.y < 20 - cd / 2.0) {
                if (pos.y < - cd / 1.7) {
                    return { VoxelTypeIndex::V_DARK_GRASS, d }; // grass bottom layer
                }
                else {
                    return { VoxelTypeIndex::V_GRASS, d }; // grass top layer
                }
            }
            else {
                return { VoxelTypeIndex::V_STONE, d }; // upper mountains
            }
        }

    }

private:
    dbl CaveDepth(Vector3 pos) {
        using namespace Utilites;
        dbl caveFactor = 0.;
        auto& seeds = params.seed.CaveSeeds;
        num n = seeds.size();
        dbl s = pow(sMultipiler, -n);
        dbl h = 1.;
        for (size_t i = 0; i < n; i++) {
            h /= 2;
            s *= sMultipiler;
            caveFactor += PerlinNoise(pos * s * params.caveScale, seeds[i]) * h;
        }
        return (caveFactor - 1 + params.caveIntensity);
    }
};

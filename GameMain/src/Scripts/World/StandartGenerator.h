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
#include "Voxels\VoxelSand.h"
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

    VoxelData GetVoxelData(Vector3 pos, dbl groundAltitude, bool transparancy) {
        float d = (float)groundAltitude;

        auto py = (float)pos.y;
        if (transparancy) {
            d = Math::Max(-d, py - waterLevel);
        }

        if (groundAltitude > 0) {
            if (py < waterLevel) {
                return { VoxelTypeIndex::V_WATER, d}; // lakes
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

        if (py > -10.f * d && py < 5 *  d) {
            return { VoxelTypeIndex::V_DIRT, d }; // partially underground layer
        }
        else if (d < -1.7f && cd < 2) {
            return { VoxelTypeIndex::V_DARK_STONE, d }; // fully underground
        }

        if (cd > -5) {
            if (py > 30.f - cd) {
                return { VoxelTypeIndex::V_SNOW, d }; // mountain tops
            }
            else if (py < 20.f && cd > 0) {
                return { VoxelTypeIndex::V_VOID, d }; // caves
            }
            else {
                if (py >= waterLevel - 1&& py <= waterLevel + 1 && cd <= -2) {
                    return  { VoxelTypeIndex::V_SAND, d };
                }
                return { VoxelTypeIndex::V_STONE, d }; // middle mountains
            }
        }
        else {
            if (py < 20.f - cd / 2.0f) {
                if (py < - cd / 1.7f) {
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
        if (pos.y < waterLevel - 10) {
            return (caveFactor - 1 + params.caveIntensity);
        }
        else {
            return (caveFactor - 1 + params.caveIntensity / 1.5 );
        }
    }
};

#pragma once

#include <GameEngine\Util.h>
#include <GameEngine\Vectors.h>
#include <GameEngine\Vectors\Vector2i.h>
#include "Util.h"

namespace Utilites {
    using std::vector;

    int Hash(int x);

    int Hash(int x, int y);

    int Hash(int x, int y, int z);

    int Hash(int x, int y, int z, int seed);

    dbl NumToDoubleNorm(num x);

    dbl Rand(Vector3 pos, int seed);

    dbl Rand(Vector2 pos, int seed);

    dbl Interpolate(dbl arr[], Vector2 s);

    dbl Interpolate(dbl arr[], Vector3 s);

    dbl PerlinNoise(Vector2 pos, int seed);

    dbl PerlinNoise(Vector3 pos, int seed);
}

#pragma once

#include <GameEngine\Util.h>
#include <GameEngine\Vectors.h>
#include <GameEngine\Math.h>
#include <GameEngine\Vectors\Vector2i.h>

namespace Utilites {
    using std::vector;

    dbl Cosine(dbl x) { return -cos(x * Game::Math::PI) * .5 + .5; };

    int Hash(int x) {
        x += (x << 10);
        x ^= (x >> 6);
        x += (x << 3);
        x ^= (x >> 11);
        x += (x << 15);
        return x;
    }

    int Hash(int x, int y) {
        return Hash(x ^ Hash(y));
    }

    int Hash(int x, int y, int z) {
        return Hash(x ^ Hash(y) ^ Hash(z));
    }

    int Hash(int x, int y, int z, int seed) {
        return Hash(x ^ Hash(y) ^ Hash(z) ^ Hash(seed));
    }

    dbl NumToDoubleNorm(num x) {
        static const num mantissa = 0x000fffffffffffff;
        static const num one = 0x3FF0000000000000;

        x &= mantissa;
        x |= one;

        auto f = *((double*)&x);

        return f - 1.;
    }

    dbl Rand(Vector3 pos, int seed) {
        return NumToDoubleNorm(Hash(pos[0], pos[1], pos[2], seed));
    }

    dbl Rand(Vector2 pos, int seed) {
        return NumToDoubleNorm(Hash(pos.x, pos.y, seed));
    }

    dbl Interpolate(vector<dbl> arr, Vector2 s) {
        s.x = Cosine(s.x);
        s.y = Cosine(s.y);
        return ((arr[0] * (1 - s.x) + arr[1] * s.x) * (1 - s.y) +
            (arr[2] * (1 - s.x) + arr[3] * s.x) * s.y);
    }

    dbl Interpolate(vector<dbl> arr, Vector3 s) {
        s.x = Cosine(s.x);
        s.y = Cosine(s.y);
        s.z = Cosine(s.z);
        return ((arr[0] * (1 - s.x) + arr[1] * s.x) * (1 - s.y) +
            (arr[2] * (1 - s.x) + arr[3] * s.x) * s.y) * (1 - s.z) +
            ((arr[4] * (1 - s.x) + arr[5] * s.x) * (1 - s.y) +
                (arr[6] * (1 - s.x) + arr[7] * s.x) * s.y) * s.z;
    }

    dbl PerlinNoise(Vector2 pos, int seed) {
        Vector2 IntPos = { floor(pos.x), floor(pos.y) };

        Vector2 s = pos - IntPos;

        auto arr = vector<dbl>(4);
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                arr[i * 2 + j] = Rand(IntPos + Vector2(j, i), seed);

        auto v = Interpolate(arr, s);

        return v;
    }

    dbl PerlinNoise(Vector3 pos, int seed) {
        Vector3 IntPos = { floor(pos.x), floor(pos.y), floor(pos.z) };

        Vector3 s = pos - IntPos;

        auto arr = vector<dbl>(8);
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                    arr[(i * 2 + j)*2 + k] = Rand(IntPos + Vector3(k, j, i), seed);

        auto v = Interpolate(arr, s);

        return v;
    }
}

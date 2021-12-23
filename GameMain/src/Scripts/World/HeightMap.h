#pragma once
#include <GameEngine\Vectors\Vector2i.h>
#include "WorldGenerator.h"

class Chunk;

class HeightMap {
private:
    std::vector<dbl> map{};
    dbl minh, maxh;
public:
    HeightMap(WorldGenerator& worldGenerator, Vector2i pos, bool generateNormals = false);

    dbl Get(int x, int y);

    dbl Get(Vector2i pos) {
        return Get(pos.x, pos.y);
    }

    const dbl MinHeight() { return minh; }
    const dbl MaxHeight() { return maxh; }
};


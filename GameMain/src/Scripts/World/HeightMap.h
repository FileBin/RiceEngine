#pragma once
#include <GameEngine\Vectors\Vector2i.h>
#include "WorldGenerator.h"

class Chunk;

class HeightMap {
private:
    std::vector<let> map{};
    let minh, maxh;
public:
    HeightMap(WorldGenerator& worldGenerator, Vector2i pos, bool generateNormals = false);

    let Get(int x, int y);

    let Get(Vector2i pos) {
        return Get(pos.x, pos.y);
    }

    const let MinHeight() { return minh; }
    const let MaxHeight() { return maxh; }
};


#pragma once
#include <GameEngine\Vectors\Vector2i.h>
#include "WorldGenerator.h"

class Chunk;

class HeightMap {
private:
    std::vector<dbl> map{};
    dbl minh, maxh;
    long counter = 0l;
    std::shared_mutex mut;
public:
    HeightMap(WorldGenerator& worldGenerator, Vector2i pos, bool generateNormals = false);

    ~HeightMap() {
        std::unique_lock lock(mut);
    }

    std::shared_mutex& GetMutex() { return mut; }

    long GetCount() {
        //std::lock_guard lock(mut);
        return counter;
    }

    void Increment() {
        //std::lock_guard lock(mut);
        counter++;
    }

    void Decrement() {
        //std::lock_guard lock(mut);
        counter--;
    }

    dbl Get(int x, int y);

    dbl Get(Vector2i pos) {
        return Get(pos.x, pos.y);
    }

    const dbl MinHeight() { return minh; }
    const dbl MaxHeight() { return maxh; }
};


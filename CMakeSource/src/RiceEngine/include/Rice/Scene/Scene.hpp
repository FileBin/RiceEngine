#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class Scene;
typedef RefPtr<Scene> pScene;

NSP_ENGINE_END

#pragma once

#include "Object.hpp"
#include "Rice/Engine/Engine.hpp"

NSP_ENGINE_BEGIN

class Scene {
public:
    void setup(pEngine engine);
    void init();

    void render();

    void close();

    bool isLoaded();
};

NSP_ENGINE_END
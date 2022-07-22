#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class SceneRender;
typedef RefPtr<SceneRender> pSceneRender;

NSP_ENGINE_END

#pragma once

#include "Scene.hpp"
#include "../Engine/Engine.hpp"

NSP_ENGINE_BEGIN

class SceneRender {
    pScene scene;
    pEngine engine;

    SceneRender(pScene scene, pEngine engine);
    void registerMesh();
    void unregisterMesh();
    void draw();
};

NSP_ENGINE_END
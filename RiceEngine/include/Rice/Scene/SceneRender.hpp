#include "../stdafx.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include <cstddef>

NSP_ENGINE_BEGIN

class SceneRender;
typedef RefPtr<SceneRender> pSceneRender;

NSP_ENGINE_END

NSP_COMPONENTS_BEGIN

class Camera;
typedef RefPtr<Camera> pCamera;

NSP_COMPONENTS_END

#pragma once

#include "Scene.hpp"
#include "../Engine/Engine.hpp"
#include "../GL/RenderingMesh.hpp"

NSP_ENGINE_BEGIN

class SceneRender {
private:
    pScene scene;
    pEngine engine;
    RegisterCollection<Graphics::RenderingMesh, size_t> mesh_collection;
    vec<Graphics::pCommandBuffer> commandBuffers;

    uint update(Components::pCamera camera);
    vec<Graphics::pCommandBuffer> getCmds(uint count);

public:
    SceneRender(pScene scene, pEngine engine);
    void registerMesh(Graphics::pRenderingMesh mesh);
    void unregisterMesh(Graphics::pRenderingMesh mesh);
    void draw(Components::pCamera camera);
};

NSP_ENGINE_END
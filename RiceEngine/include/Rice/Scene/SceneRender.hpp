#include "../stdafx.hpp"
#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include <cstddef>

NSP_ENGINE_BEGIN
class SceneRender;
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN
class Camera;
NSP_COMPONENTS_END

#pragma once

#include "Scene.hpp"
#include "../Engine/Engine.hpp"

NSP_GL_BEGIN
class RenderingMesh;
NSP_GL_END

NSP_ENGINE_BEGIN

class SceneRender {
private:
    wptr<Scene> scene;
    ptr<Engine> engine;
    Util::RegisterCollection<Graphics::RenderingMesh, size_t> mesh_collection;

    uint update(ptr<Components::Camera> camera);
    vec<ptr<Graphics::CommandBuffer>> getCmds(uint count);

public:
    SceneRender(ptr<Scene> scene, ptr<Engine> engine);
    void registerMesh(ptr<Graphics::RenderingMesh> mesh);
    void unregisterMesh(ptr<Graphics::RenderingMesh> mesh);
    void draw(ptr<Components::Camera> camera);
    void cleanup();
};

NSP_ENGINE_END
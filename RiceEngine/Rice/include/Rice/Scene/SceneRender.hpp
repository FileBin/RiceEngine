#include "Rice/GL/Material.hpp"
#include "Rice/stdafx.hpp"

NSP_ENGINE_BEGIN
class SceneRender;
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN
class Camera;
NSP_COMPONENTS_END

#pragma once

#include "ClientScene.hpp"
#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/GL/RenderingMesh.hpp"

NSP_GL_BEGIN
class RenderingMesh;
NSP_GL_END

NSP_ENGINE_BEGIN

class SceneRender : public enable_ptr<SceneRender> {
  private:
    map<String, ptr<Graphics::Shader>> shaders;
    map<String, ptr<Graphics::Material>> materials;

    wptr<ClientScene> scene;
    wptr<ClientEngine> engine;
    Util::RegisterCollection<Graphics::RenderingMesh, size_t> mesh_collection;
    ptr<Graphics::CommandBuffer> main_cmd, clear_cmd;

    bool need_rebuild = true;

    void updateCmd();
    void rebuildCmd();

    uint update(ptr<Components::Camera> camera);

  public:
    SceneRender(ptr<ClientScene> scene, ptr<ClientEngine> engine);
    void registerMesh(ptr<Graphics::RenderingMesh> mesh);
    void unregisterMesh(ptr<Graphics::RenderingMesh> mesh);
    void draw(ptr<Components::Camera> camera);
    void cleanup();

    ptr<Graphics::Shader> getShader(String name);

    ptr<Graphics::Material> getMaterial(String name);

    ptr<Graphics::Shader>
    getOrCreateShader(String name, std::function<void(ptr<Graphics::Shader>)> shader_creator);
    ptr<Graphics::Material> getOrCreateMaterial(
        String name,
        std::function<ptr<Graphics::Material>(ptr<SceneRender>)> material_factory);

    ptr<Graphics::GraphicsManager> getGraphicsManager();

    ptr<ClientEngine> getEngine();
    ptr<ClientScene> getScene();
};

NSP_ENGINE_END
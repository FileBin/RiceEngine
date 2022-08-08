#include "../stdafx.hpp"

NSP_ENGINE_BEGIN
class ClientScene;
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN
class Camera;
NSP_COMPONENTS_END

#pragma once

#include "Object.hpp"
#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/GL/Material.hpp"
#include "SceneBase.hpp"
#include "SceneRender.hpp"

NSP_ENGINE_BEGIN

class ClientScene : virtual public SceneBase {
  protected:
    ClientScene();

    ptr<ClientScene> shared_client_scene();

  public:
    void setup(ptr<EngineBase> engine) override;
    void render();

    void close() override;

    ptr<SceneRender> getSceneRender();
    ptr<ClientEngine> getClientEngine();

    void setActiveCamera(ptr<Components::Camera> camera);
    ptr<Components::Camera> getActiveCamera();

  private:
    ptr<SceneRender> scene_render;
    ptr<Components::Camera> active_camera;
};

NSP_ENGINE_END
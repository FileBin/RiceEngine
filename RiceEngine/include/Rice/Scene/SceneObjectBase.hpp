#include "Rice/stdafx.hpp"

NSP_ENGINE_BEGIN
class SceneBase;
class SceneRender;
class Object;
class EngineBase;
class ClientEngine;
class ClientScene;
NSP_ENGINE_END

#pragma once
NSP_ENGINE_BEGIN

class SceneObjectBase {
  private:
    friend class SceneBase;
    wptr<SceneBase> scene;
    UUID uuid;

  protected:
    SceneObjectBase();
    virtual ~SceneObjectBase();

    ptr<EngineBase> getEngine();
    ptr<ClientEngine> getClientEngine();
    ptr<SceneRender> getSceneRender();

  public:
    ptr<SceneBase> getScene();
    ptr<ClientScene> getClientScene();
    UUID getUUID();
};

NSP_ENGINE_END
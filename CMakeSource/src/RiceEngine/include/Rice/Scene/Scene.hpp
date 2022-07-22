#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class Scene;
typedef RefPtr<Scene> pScene;

NSP_ENGINE_END

#pragma once

#include "Object.hpp"
#include "../Engine/Engine.hpp"
#include "SceneRender.hpp"

NSP_ENGINE_BEGIN

class Scene {
    friend class SceneRender;
  public:
    void setup(pEngine engine);
    virtual void init() = 0;

    void render();

    virtual void close() {};

    bool isLoaded();

    pObject getObject(UUID uuid);

    pObject createEmpty();

  private:
    RegisterCollection<pObject, UUID> all_objects;

    friend struct ObjectData;

    //NOTE: can throw exception if UUID is exists in the scene
    void pushObjectWithUUID(pObject original, UUID uuid);
};

NSP_ENGINE_END
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

    pObject getObject(UUID uuid);

    pObject createEmpty();

  private:
    RegisterCollection<pObject, UUID> all_objects;\

    friend struct ObjectData;

    //NOTE: can throw exception if UUID is exists in the scene
    void pushObjectWithUUID(pObject original, UUID uuid);
};

NSP_ENGINE_END
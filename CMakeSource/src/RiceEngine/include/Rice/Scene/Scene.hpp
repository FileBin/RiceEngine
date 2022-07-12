#include "../stdafx.hpp"
#include "BetterCpp/Objects/PtrTypes.hpp"

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
    typedef uint64_t UUID;
    void setup(pEngine engine);
    void init();

    void render();

    void close();

    bool isLoaded();

    pObject getObject(UUID uuid);

    //NOTE: can throw exception if UUID is exists in the scene
    void pushObjectWithUUID(pObject original, UUID uuid);

    pObject createEmpty();

  private:
    RegisterCollection<pObject, UUID> all_objects;
};

NSP_ENGINE_END
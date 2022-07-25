#include "../stdafx.hpp"
#include "BetterCpp/Functions.hpp"
#include <cstdint>

NSP_ENGINE_BEGIN

class Scene;
typedef RefPtr<Scene> pScene;

NSP_ENGINE_END

#pragma once

#include "../Engine/Engine.hpp"
#include "Object.hpp"
#include "SceneRender.hpp"

NSP_ENGINE_BEGIN

class Scene : public EnableThisRefPtr<Scene> {
    friend class SceneRender;
    friend class Components::Component;

  protected:
    pEngine getEngine();
  public:
    virtual ~Scene() {}
    void setup(pEngine engine);
    virtual void init() = 0;

    void update();
    void render();

    virtual void close(){};

    bool isLoaded() { return loaded; }

    void setActiveCamera(Components::pCamera camera);

    pObject getObject(UUID uuid);

    pObject createEmpty();

    void destroyObject(UUID uuid);

  private:
    RegisterCollection<Object, uint64_t> all_objects;
    pObject root = new_ref<Object>(refptr_this());

    pEngine engine;
    pSceneRender scene_render;
    Components::pCamera active_camera;

    bool loaded = false;
    friend struct ObjectData;

    // NOTE: can throw exception if UUID is exists in the scene
    void pushObjectWithUUID(pObject original, UUID uuid);
};

NSP_ENGINE_END
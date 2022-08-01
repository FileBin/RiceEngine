#include "../stdafx.hpp"
#include "Rice/Util/Event.hpp"
#include <cstdint>

NSP_ENGINE_BEGIN
class Scene;
NSP_ENGINE_END

#pragma once

#include "../Engine/Engine.hpp"
#include "Object.hpp"
#include "SceneRender.hpp"

NSP_ENGINE_BEGIN

class Scene : public enable_ptr<Scene> {
    friend class SceneRender;
    friend class Components::Component;
    friend class Object;

  protected:
    ptr<SceneRender> getSceneRender();
    ptr<Engine> getEngine();
    Scene() = default;

  public:
    struct Events {
        ptr<Event<>> processEvents = Event<>::create();
        ptr<Event<>> update = Event<>::create();

        ptr<Event<>> close = Event<>::create();
    } events;

    virtual ~Scene() { close(); }
    void setup(ptr<Engine> engine);
    virtual void init() = 0;
    void load();

    void update();
    void render();

    virtual void close(){};

    bool isLoaded() { return loaded; }

    void setActiveCamera(ptr<Components::Camera> camera);

    ptr<Object> getObject(UUID uuid);

    ptr<Object> createEmpty(String name);
    ptr<Object> createEnabled(String name);

    void destroyObject(UUID uuid);

  private:
    uint64_t uuid_counter = 0;
    UUID getNextUUID();

    Util::RegisterCollection<Object, uint64_t> all_objects;
    ptr<Object> root;

    ptr<Engine> engine;
    ptr<SceneRender> scene_render;
    ptr<Components::Camera> active_camera;

    bool loaded = false;
    friend struct ObjectData;

    // NOTE: can throw exception if UUID is exists in the scene
    void pushObjectWithUUID(ptr<Object> original, UUID uuid);
};

NSP_ENGINE_END
#include "Rice/stdafx.hpp"

NSP_ENGINE_BEGIN
class SceneBase;
class EngineBase;
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN
class Component;
NSP_COMPONENTS_END

#pragma once
#include "SceneObjectBase.hpp"

NSP_ENGINE_BEGIN

class SceneBase : public enable_ptr<SceneBase> {
    friend class SceneObjectBase;
    friend class Components::Component;
    friend class Object;

  protected:
    ptr<EngineBase> getEngine();
    SceneBase() = default;

  public:
    struct Events {
        ptr<Event<>> processEvents = Event<>::create();
        ptr<Event<>> update = Event<>::create();

        ptr<Event<>> close = Event<>::create();
    } events;

    virtual ~SceneBase() { close(); }
    virtual void setup(ptr<EngineBase> engine);
    virtual void init() = 0;
    void load();

    void update();

    virtual void close();
    virtual void onclose(){};

    bool isLoaded() { return loaded; }

    ptr<SceneObjectBase> getRegistered(UUID uuid);

    ptr<Object> createEmpty(String name);
    ptr<Object> createEnabled(String name);

    void destroyObject(UUID uuid);

  private:
    uint64_t uuid_counter = 0;
    UUID getNextUUID();

    void Register(ptr<SceneObjectBase> object);

    umap<uint64_t, ptr<SceneObjectBase>> all_objects;
    ptr<Object> root;

    ptr<EngineBase> engine;

    bool loaded = false;
    friend struct ObjectData;

    // NOTE: can throw exception if UUID is exists in the scene
    void pushObjectWithUUID(ptr<Object> original, UUID uuid);
};

NSP_ENGINE_END
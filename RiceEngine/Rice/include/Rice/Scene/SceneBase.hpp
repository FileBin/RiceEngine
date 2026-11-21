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

    /*
     * One frame of scene logic: root->preUpdate() then root->update(), followed
     * by the deferred-mutation drain point (see below).
     */
    void update();

    virtual void close();
    virtual void onclose(){};

    bool isLoaded() { return loaded; }

    ptr<SceneObjectBase> getRegistered(UUID uuid);

    /*
     * Visits every object registered in this scene (the root included), once
     * each, in unspecified order. Main-thread only, and only outside an update
     * pass: it is the read-only counterpart of createEmpty()/destroy(), used by
     * the renderer to find scene-wide resources such as the
     * Components::DirectionalLight. Do not create, destroy or re-parent objects
     * from the visitor - the map is being iterated.
     */
    void forEachObject(const std::function<void(ptr<SceneObjectBase>)> &visitor);

    ptr<Object> createEmpty(String name);
    ptr<Object> createEnabled(String name);

    void destroyObject(UUID uuid);

    /*
     * ---- mutating the scene graph from a script's onUpdate ----------------
     *
     * update() iterates the object tree and holds each Object's shared_mutex
     * shared on this thread while a component's onUpdate runs. Creating a child
     * from there would take a unique_lock on that same mutex (self-deadlock)
     * and push_back into the very vectors being iterated (iterator
     * invalidation). Both are therefore deferred while isUpdateRunning() is
     * true:
     *
     *  - createEmpty()/createEnabled() still construct and register the object
     *    immediately: the returned pointer has a valid uuid and scene, and can
     *    be configured (addComponent) right away.
     *  - addComponent() on any object is deferred the same way, so a component
     *    list is never modified while it is being iterated.
     *  - the deferred actions are applied at ONE point of the frame: at the end
     *    of update(), right after root->update() returns and before render(),
     *    in the order they were requested. Objects are attached to their parent
     *    there and enabled.
     *  - the new objects/components become active in the next frame's
     *    preUpdate, i.e. an object created during onUpdate appears in the tree
     *    (and is drawn) at the next frame boundary.
     *
     * Destruction is flag based and equally well defined: destroy() only marks
     * the object; the next preUpdate pass tears it down (components disabled ->
     * meshes unregistered, uuid released, children destroyed) and removes it
     * from its parent's child list. So a destroy issued during onUpdate takes
     * effect at the beginning of the next frame, at most one frame later.
     *
     * Everything here is main-thread only: this is not a thread-safe scene
     * graph, it is a safe way to mutate it from the update pass.
     */
    bool isUpdateRunning() const { return update_running; }

  private:
    uint64_t uuid_counter = 0;
    UUID getNextUUID();

    void Register(ptr<SceneObjectBase> object);
    void Unregister(ptr<SceneObjectBase> object);

    umap<uint64_t, ptr<SceneObjectBase>> all_objects;
    ptr<Object> root;

    ptr<EngineBase> engine;

    bool loaded = false;
    friend struct ObjectData;

    // Deferred-mutation queue, drained by update() after root->update().
    bool update_running = false;
    vec<std::function<void()>> deferred_actions;
    void enqueueDeferred(std::function<void()> action);
    void drainDeferred();

    // NOTE: can throw exception if UUID is exists in the scene
    void pushObjectWithUUID(ptr<Object> original, UUID uuid);
};

NSP_ENGINE_END

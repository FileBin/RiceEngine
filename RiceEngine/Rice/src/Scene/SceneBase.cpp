#include "Rice/Engine/EngineBase.hpp"
#include "Rice/Scene/Object.hpp"
#include "Rice/Scene/SceneObjectBase.hpp"
#include "Rice/Util/Util.hpp"

NSP_ENGINE_BEGIN

ptr<EngineBase> SceneBase::getEngine() { return engine; }

void SceneBase::Register(ptr<SceneObjectBase> object) {
    auto key = getNextUUID();
    object->uuid = key;
    object->scene = weak_from_this();
    all_objects[key.getVal()] = object;
}

void SceneBase::Unregister(ptr<SceneObjectBase> object) {
    all_objects.erase(object->uuid.getVal());
}

void SceneBase::setup(ptr<EngineBase> en) {
    engine = en;
    root = ptr<Object>(new Object("root"));
    Register(root);
    // TODO InitResourceManager();
}

void SceneBase::load() {
    loaded = true;
    init();
    root->flags |= Object::Flags::ACTIVE;
}

void SceneBase::update() {
    struct Guard {
        bool &flag;
        ~Guard() { flag = false; }
    } guard{update_running};

    update_running = true;
    root->preUpdate();
    root->update();
    // Single drain point: the update pass is over, no tree iteration is in
    // flight and no shared lock is held, so attaching objects and pushing
    // components is safe here.
    update_running = false;
    drainDeferred();
}

void SceneBase::enqueueDeferred(std::function<void()> action) {
    deferred_actions.push_back(std::move(action));
}

void SceneBase::drainDeferred() {
    // Actions may enqueue more actions (a create followed by addComponent is
    // two entries); run until the queue is empty, keeping FIFO order so the
    // object is attached before anything is added to it.
    while (!deferred_actions.empty()) {
        vec<std::function<void()>> batch;
        batch.swap(deferred_actions);
        for (auto &action : batch) {
            if (action) action();
        }
    }
}

void SceneBase::close() {
    loaded = false;
    root->forceDisable();
}

ptr<Object> SceneBase::createEmpty(String name) {
    return root->createEmpty(name);
}
ptr<Object> SceneBase::createEnabled(String name) {
    return root->createEnabled(name);
}

ptr<SceneObjectBase> SceneBase::getRegistered(UUID uuid) {
    try {
        return all_objects.at(uuid.getVal());
    } catch (const std::out_of_range &e) {
        THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
    }
}

UUID SceneBase::getNextUUID() { return UUID(uuid_counter++); }

NSP_ENGINE_END
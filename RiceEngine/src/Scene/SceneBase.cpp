#include "Rice/Engine/EngineBase.hpp"
#include "Rice/Scene/Object.hpp"

NSP_ENGINE_BEGIN

ptr<EngineBase> SceneBase::getEngine() { return engine; }

void SceneBase::Register(ptr<SceneObjectBase> object) {
    auto key = getNextUUID();
    object->uuid = key;
    object->scene = weak_from_this();
    all_objects[key.getVal()] = object;
}

void SceneBase::setup(ptr<EngineBase> en) {
    engine = en;
    root = ptr<Object>(new Object("root"));
    Register(root);
    // TODO InitResourceManager();
}

void SceneBase::load() {
    init();
    root->flags |= Object::Flags::ENABLED;
    loaded = true;
}

void SceneBase::update() {
    root->preUpdate();
    root->update();
}

void SceneBase::close() { root->forceDisable(); }

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
#include "Rice/Scene/Object.hpp"
#include "Rice/Scene/SceneRender.hpp"
#include "Rice/Util/String.hpp"
#include "Rice/Util/Util.hpp"
#include "Rice/defines.h"
#include <Rice/Scene/Scene.hpp>

NSP_ENGINE_BEGIN

ptr<Engine> Scene::getEngine() { return engine; }

void Scene::setup(ptr<Engine> en) {
    engine = en;
    scene_render = new_ptr<SceneRender>(shared_from_this(), engine);
    root = ptr<Object>(new Object(shared_from_this(), "root", getNextUUID()));
    // TODO InitResourceManager();
}

void Scene::load() {
    init();
    events.update->subscribe(root->updateRegistration,
                             [this]() { root->update(); });
    loaded = true;
}

void Scene::update() {
    events.processEvents->invoke();
    events.update->invoke();
}

void Scene::render() {
    if (active_camera) {
        scene_render->draw(active_camera);
    } else {
        // TODO draw no camera label
    }
}

ptr<Object> Scene::createEmpty(String name) { return root->createEmpty(name); }
ptr<Object> Scene::createEnabled(String name) {
    return root->createEnabled(name);
}

ptr<Object> Scene::getObject(UUID uuid) {
    return all_objects.getElemAt(uuid.getVal());
}

ptr<SceneRender> Scene::getSceneRender() { return scene_render; }

void Scene::setActiveCamera(ptr<Components::Camera> camera) {
    active_camera = camera;
}

UUID Scene::getNextUUID() { return UUID(uuid_counter++); }

NSP_ENGINE_END
#include <Rice/Scene/Scene.hpp>

NSP_ENGINE_BEGIN

ptr<Engine> Scene::getEngine() {
    return engine;
}

void Scene::setup(ptr<Engine> en) {
    engine = en;
    scene_render = new_ptr<SceneRender>(shared_from_this(), engine);
    root = Object::create(shared_from_this());
    // TODO InitResourceManager();
}

void Scene::update() {
    // TODO root->update();
}

void Scene::render() {
    if (active_camera) {
        scene_render->draw(active_camera);
    } else {
        // TODO draw no camera label
    }
}

ptr<Object> Scene::createEmpty() {
    //FIXME add uuid to the object
    return root->createEmpty();
}

ptr<Object> Scene::getObject(UUID uuid) {
    return all_objects.getElemAt(uuid.getVal());
}

void Scene::setActiveCamera(ptr<Components::Camera> camera) {
    active_camera = camera;
}

NSP_ENGINE_END
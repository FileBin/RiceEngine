#include "Rice/Scene/SceneRender.hpp"
#include "pch.h"

#include <Rice/Scene/Scene.hpp>

NSP_ENGINE_BEGIN

void Scene::setup(pEngine en) {
    engine = en;
    scene_render = new_ref<SceneRender>(refptr_this(), engine);
    // TODO InitResourceManager();
}

void Scene::update() {
    // TODO root->update();
}

void Scene::render() {
    if (active_camera.isNotNull()) {
        scene_render->draw(active_camera);
    } else {
        // TODO draw no camera label
    }
}

pObject Scene::createEmpty() {
    return root->createEmpty();
}

void Scene::setActiveCamera(Components::pCamera camera) {
    active_camera = camera;
}

NSP_ENGINE_END
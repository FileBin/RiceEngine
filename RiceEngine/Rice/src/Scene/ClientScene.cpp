#include "pch.h"
#include <Rice/Scene/ClientScene.hpp>
NSP_ENGINE_BEGIN

ClientScene::ClientScene() : SceneBase() {}

ptr<ClientScene> ClientScene::shared_client_scene() {
    return std::dynamic_pointer_cast<ClientScene>(shared_from_this());
}

void ClientScene::setup(ptr<EngineBase> en) {
    SceneBase::setup(en);
    scene_render =
        new_ptr<SceneRender>(shared_client_scene(), getClientEngine());
}

void ClientScene::render() {
    if (active_camera) {
        scene_render->draw(active_camera);
    } else {
        // TODO draw no camera label
    }
}

void ClientScene::close() {
    SceneBase::close();
    scene_render->cleanup();
}

ptr<ClientEngine> ClientScene::getClientEngine() {
    return std::dynamic_pointer_cast<ClientEngine>(getEngine());
}

ptr<SceneRender> ClientScene::getSceneRender() { return scene_render; }

void ClientScene::setActiveCamera(ptr<Components::Camera> camera) {
    active_camera = camera;
}

ptr<Components::Camera> ClientScene::getActiveCamera() { return active_camera; }
NSP_ENGINE_END
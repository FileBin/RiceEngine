#include "Rice/Engine/ClientCore.hpp"
#include "pch.h"

#include "Rice/Engine/ClientEngine.hpp"
#include "Rice/Engine/CoreBase.hpp"
#include "Rice/Engine/EngineBase.hpp"
#include "Rice/Scene/ClientScene.hpp"
#include <memory>
NSP_ENGINE_BEGIN

ClientEngine::ClientEngine(ptr<ClientCore> core)
    : EngineBase(std::static_pointer_cast<CoreBase>(core)) {}
ptr<ClientEngine> ClientEngine::create(ptr<ClientCore> core) {
    return ptr<ClientEngine>(new ClientEngine(core));
}
ClientEngine::~ClientEngine() {}

void ClientEngine::loadScene(ptr<ClientScene> scene) {
    lock_core()->loadScene(scene);
}

void ClientEngine::loadScene(ptr<SceneBase> scene) {
    auto client_scene = std::dynamic_pointer_cast<ClientScene>(scene);
    if (client_scene) {
        loadScene(client_scene);
    }
    THROW_EXCEPTION("Scene is not a ClientScene! Maybe you use Server "
                         "Scene? \n Try use #ifndef SERVER_BUILD");
}

ptr<ClientScene> ClientEngine::getActiveScene() {
    return lock_client_core()->activeScene;
}

dbl ClientEngine::getAspectRatio() {
    auto size = lock_client_core()->wnd->getSize();
    return (dbl)size.x / size.y;
}

ptr<ClientCore> ClientEngine::lock_client_core() {
    return std::dynamic_pointer_cast<ClientCore>(lock_core());
}

ptr<Graphics::GraphicsManager> ClientEngine::getGraphicsManager() {
    return lock_client_core()->graphics_manager;
}

ptr<InputManager> ClientEngine::getInputManager() {
    return lock_client_core()->wnd->getInputManager();
}

NSP_ENGINE_END
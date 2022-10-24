#include "../stdafx.hpp"

NSP_ENGINE_BEGIN
class ClientEngine;
NSP_ENGINE_END

#pragma once
#include "ClientCore.hpp"
#include "Rice/Engine/EngineBase.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Shader.hpp"
//#include "../AL/SoundManager.hpp" //TODO

#include "Rice/Scene/ClientScene.hpp"

NSP_ENGINE_BEGIN

class ClientEngine : public virtual EngineBase {
  private:
    ClientEngine(ptr<ClientCore> core);

  public:
    static ptr<ClientEngine> create(ptr<ClientCore> core);
    ~ClientEngine();

    void loadScene(ptr<ClientScene> scene);
    void loadScene(ptr<SceneBase> scene) override;

    ptr<ClientScene> getActiveScene();

    dbl getAspectRatio();

    ptr<Graphics::GraphicsManager> getGraphicsManager();
    ptr<InputManager> getInputManager();

  private:
    ptr<ClientCore> lock_client_core();
};

NSP_ENGINE_END

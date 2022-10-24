#include "../stdafx.hpp"
#include "Rice/Engine/InputManager.hpp"

NSP_ENGINE_BEGIN
class EngineBase;
NSP_ENGINE_END

#pragma once
#include "CoreBase.hpp"
#include "Rice/GL/GraphicsManager.hpp"
#include "Rice/GL/Material.hpp"
#include "Rice/GL/Shader.hpp"
//#include "../AL/SoundManager.hpp" //TODO

#include "Rice/Scene/SceneBase.hpp"

NSP_ENGINE_BEGIN

class EngineBase : public enable_ptr<EngineBase> {
  protected:
    EngineBase(ptr<CoreBase> core);

  public:
    virtual ~EngineBase();

    virtual void loadScene(ptr<SceneBase> scene) = 0;

    dbl getFixedDeltaTime();
    dbl getDeltaTime();
    dbl getTime();

  protected:
    ptr<CoreBase> lock_core();

    wptr<CoreBase> core;
};

NSP_ENGINE_END

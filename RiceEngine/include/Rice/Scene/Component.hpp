#include "../stdafx.hpp"
#include "Rice/Util/Interfaces.hpp"
#include "Rice/defines.h"
#include <limits>
#include <memory>

NSP_COMPONENTS_BEGIN

class Component;

NSP_COMPONENTS_END

#pragma once

NSP_ENGINE_BEGIN
class Object;
class Engine;
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN

class Component : public enable_ptr<Component> , public IPackable<data_t>{
    friend class Rice::Object;

  protected:
    ptr<Engine> getEngine();

    wptr<Object> object;
    virtual void onInit();
    virtual void onEnable();
    virtual void onDisable();
    virtual void start();
    virtual void preUpdate();
    virtual void update();

  protected:
    Component() = default;

  public:
    ptr<Object> getObject();

    void enable();
    void disable();

    bool isEnabled();

    virtual ~Component() {}
};

NSP_COMPONENTS_END
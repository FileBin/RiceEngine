#include "../stdafx.hpp"
#include "Rice/Scene/SceneObjectBase.hpp"
#include "Rice/Util/Event.hpp"

NSP_COMPONENTS_BEGIN

class Component;

NSP_COMPONENTS_END

#pragma once

NSP_ENGINE_BEGIN
class Object;
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN

class Component : public SceneObjectBase,
                  public enable_ptr<Component>,
                  public IPackable<data_t> {
    friend class Rice::Object;

  private:
    wptr<Object> object;

    void init(ptr<Object> object);

  protected:
    bool isSceneLoaded();

  protected:
    Component() = default;

    void onEnable() override;
    void onDisable() override;
    void onUpdate() override;
    void onPreUpdate() override;

    ptr<SceneObjectBase> getBaseParent() override;

  public:
    ptr<Object> getObject();

    ~Component() override;
};

NSP_COMPONENTS_END
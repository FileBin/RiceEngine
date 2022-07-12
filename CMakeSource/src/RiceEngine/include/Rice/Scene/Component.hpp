#include "../stdafx.hpp"

NSP_COMPONENTS_BEGIN

class Component;
typedef RefPtr<Component> pComponent;

struct ComponentData;

NSP_COMPONENTS_END

#pragma once

#include "Object.hpp"

NSP_COMPONENTS_BEGIN

class Component {
protected:
  pObject object;

  virtual void onEnable() = 0;
  virtual void start(){};
  virtual void preUpdate(){};
  virtual void update(){};

public:
  pObject getObject();

  void enable();
  void disable();

  bool isEnabled();
};

struct ComponentData {
    UUID objectUUID;

    virtual pComponent createComponent();
};

NSP_COMPONENTS_END
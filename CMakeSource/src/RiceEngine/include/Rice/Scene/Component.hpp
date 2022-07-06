#include "../stdafx.hpp"

NSP_COMPONENTS_BEGIN

class Component;
typedef RefPtr<Component> pComponent;

NSP_COMPONENTS_END

#pragma once

#include "Object.hpp"

NSP_COMPONENTS_BEGIN

class Component {
protected:
  pObject object;

  virtual void onEnable() = 0;
  virtual void Start(){};
  virtual void PreUpdate(){};
  virtual void Update(){};

public:
  pObject getObject();

  void enable();
  void disable();

  bool isEnabled();
};

NSP_COMPONENTS_END
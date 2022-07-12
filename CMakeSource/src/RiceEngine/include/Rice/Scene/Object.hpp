#include "../stdafx.hpp"
#include "BetterCpp/Objects/PtrTypes.hpp"
#include "Rice/defines.h"

NSP_ENGINE_BEGIN

class Object;
typedef RefPtr<Object> pObject;

NSP_ENGINE_END

#pragma once

#include "Component.hpp"

NSP_ENGINE_BEGIN

class Object {
  private:
    vec<Components::pComponent> components;

  public:
    void addChild(pObject object);

    template <typename T> vec<RefPtr<T>> getComponents() {
        vec<RefPtr<T>> vec = {};
        for (auto c : components) {
            auto o = dynamic_cast<T *>(c.get());
            if (o != nullptr) {
                vec.push_back(c);
            }
        }
        return vec;
    }

    template <typename T> RefPtr<T> getComponent() {
        for (auto c : components) {
            auto o = dynamic_cast<T *>(c.get());
            if (o != nullptr) {
                return c;
            }
        }
        return nullptr;
    }
};

NSP_ENGINE_END
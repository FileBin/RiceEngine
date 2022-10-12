/*
 * file: component.hpp
 * author: filebin
 * date: 2022-10-05 10:33:06
 */

#pragma once

#include "ComponentHelper.hpp"

namespace MyComponents {
using namespace Rice;
using namespace Rice::Components;

class REFLECTABLE TestComponent : public virtual SerializableComponent<TestComponent> {
    RICE_COMPONENT

  private:
    double x, y, z;
};

} // namespace MyComponents
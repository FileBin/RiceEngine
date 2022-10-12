/*
 * file: component.hpp
 * author: filebin
 * date: 2022-10-05 10:33:06
 */
#include "config.h"

#pragma once

#include "ComponentHelper.hpp"

namespace MyComponents {
using namespace Rice;
using namespace Rice::Components;

class REFLECTABLE Camera : public virtual SerializableComponent<Camera> {
    RICE_COMPONENT

  private:
    dbl fov;
    dbl near_plane;
    dbl far_plane;

  public:
    dbl getFov();
    void setFov(dbl fov);
};

} // namespace MyComponents
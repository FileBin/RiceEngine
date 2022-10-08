/*
 * file: component.hpp
 * author: filebin
 * date: 2022-10-05 10:33:06
 */

#include "config.h"
#include "serialization_helper.hpp"
#include <map>
#include <vector>

#pragma once

#include "ComponentHelper.hpp"

COMPONENT_BEGIN

REGION(private_fields) { // private serialized fields
    dbl fov;
    dbl near_plane;
    dbl far_plane;
};

REGION(public_fields){}; // public serialized fields

REGION(methods) {
  public: // write public component methods as you like
    dbl getFov();
    void setFov(dbl fov);

  protected: // write protected component methods as you like
  private:   // write private component methods as you like
};

COMPONENT_END
/*
 * file: SerializableComponent.hpp
 * author: filebin
 * date: 2022-10-06 00:52:29
 */
#pragma once

#include "Rice/Scene/Component.hpp"
#include "Rice/Util/Interfaces.hpp"
#include "Rice/defines.h"
#include "serialization_helper.hpp"
#include <nlohmann/json_fwd.hpp>

NSP_COMPONENTS_BEGIN
template <class Derived> class SerializableComponent;
NSP_COMPONENTS_END

NSP_COMPONENTS_BEGIN

template <class Derived>
class SerializableComponent : public virtual Component, public virtual ISerializable, public virtual IJsonSerializable {
  public:
    SerializableComponent() = default;
    ~SerializableComponent() {}

    data_t toBytes() override { return component_serializer<Derived>().to_bytes(dynamic_cast<Derived *>(this)); }

    nlohmann::json toJson() override { return component_serializer<Derived>().to_json(dynamic_cast<Derived *>(this)); }

  protected:
    template <typename T> friend struct ::component_serializer;
};

NSP_COMPONENTS_END
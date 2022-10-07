/*
 * file: SerializableComponent.hpp
 * author: filebin
 * date: 2022-10-06 00:52:29
 */
#pragma once

#include "Rice/Scene/Component.hpp"
#include "Rice/Util/Interfaces.hpp"
#include "Rice/defines.h"
#include "rapidjson/document.h"
#include "serialization_helper.hpp"

NSP_COMPONENTS_BEGIN
template <class Derived, typename PublicT, typename PrivateT>
class SerializableComponent;
NSP_COMPONENTS_END

NSP_COMPONENTS_BEGIN

template <class Derived, typename PublicT, typename PrivateT>
class SerializableComponent : public virtual Component,
                              public PublicT,
                              protected PrivateT,
                              public virtual ISerializable,
                              public virtual IJsonSerializable {
  public:
    SerializableComponent() = default;
    ~SerializableComponent() {}

    data_t toBytes() override {
        return component_serializer<Derived>().to_bytes(
            dynamic_cast<Derived *>(this));
    }

    rapidjson::Value toJson(rapidjson::Document::AllocatorType a) override {
        return component_serializer<Derived>().to_json(
            dynamic_cast<Derived *>(this), a);
    }

  protected:
    PrivateT *getPrivate() { return static_cast<PrivateT *>(this); }
    PublicT *getPublic() { return static_cast<PublicT *>(this); }
    template <typename T> friend struct ::component_serializer;
};

NSP_COMPONENTS_END